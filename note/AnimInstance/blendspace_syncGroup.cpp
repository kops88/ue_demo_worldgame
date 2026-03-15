	struct FSyncMarker
{
    float Time; // 同步标记时间点（秒）
};

	struct FSyncSample
{
    FString Name;
    TArray<FSyncMarker> SyncMarkers;   // 标记列表
    float PlaybackPosition = 0.0f;     // 当前播放时间（秒）
    float PlayRate = 1.0f;             // 当前播放速率
};

	struct FSyncGroup
{
    FName SyncGroupName;
    int32 LeaderIndex;                 // Leader样本索引
    TArray<FSyncSample> Samples;       // 同步样本集合


    // 每帧调用，推进所有样本的播放时间，调整Follower播放速率保持同步
    void Tick(float DeltaTime)
    {
        if (Samples.Num() == 0 || LeaderIndex < 0 || LeaderIndex >= Samples.Num())
            return;
        FSyncSample& Leader = Samples[LeaderIndex];
        // Leader以自身PlayRate推进播放时间
        Leader.PlaybackPosition += DeltaTime * Leader.PlayRate;
         // 遍历所有Follower同步Leader
        for (int32 i = 0; i < Samples.Num(); ++i)
        {
            if (i == LeaderIndex)
                continue;
            FSyncSample& Follower = Samples[i];
 
            // 找Leader最近同步标记时间点
            float LeaderSync = FindNearestSyncMarker(Leader.SyncMarkers, Leader.PlaybackPosition);
 
            // 找Follower最近同步标记时间点
            float FollowerSync = FindNearestSyncMarker(Follower.SyncMarkers, Follower.PlaybackPosition);
 
            // 计算时间差，调节播放速率(0.5为调节速度系数)
            float SyncDelta = LeaderSync - FollowerSync;
            Follower.PlayRate = 1.0f + SyncDelta * 0.5f;
 
            // 防止速度为负或过大
            Follower.PlayRate = FMath::Clamp(Follower.PlayRate, 0.5f, 1.5f);
 
            // 推进Follower播放时间
            Follower.PlaybackPosition += DeltaTime * Follower.PlayRate;
        }
    }

    float FindNearestSyncMarker(const TArray<FSyncMarker>& Markers, float CurrentTime)
    {
        float Nearest = Markers.Num() > 0 ? Markers[0].Time : 0.0f;
        float MinDist = FMath::Abs(CurrentTime - Nearest);
        for (const FSyncMarker& Marker : Markers)
        {
            float Dist = FMath::Abs(CurrentTime - Marker.Time);
            if (Dist < MinDist)
            {
                Nearest = Marker.Time;
                MinDist = Dist;
            }
        }
         return Nearest;
    }

}