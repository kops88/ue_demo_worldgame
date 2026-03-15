// 核心结构体 - BlendSpace样本
struct FBlendSample
{
    FVector2D Position;     // 参数空间的位置 (Speed, Direction等)
    FPose AnimationPose;   // 对应的骨骼姿势（动画序列采样结果）
};

	// 表示骨骼姿势的结构
struct FPose
{
    TArray<FTransform> BoneTransforms;
 
    int32 GetNumBones() const { return BoneTransforms.Num(); }
};

// BlendSpace主结构
struct FBlendSpace
{
    TArray<FBlendSample> Samples;
    FVector2D AxisMin;               // 参数最小值范围
    FVector2D AxisMax;               // 参数最大值范围
    FVector2D InputValue;            // 当前参数输入 (Speed, Direction)
    FPose OutputPose;                // 输出混合姿势

    void Evaluate()
    {
        // 1. 查找包含输入点的邻近4个样本索引 (简化实现)
        TArray<int32> NeighborIndices = FindNeighbors(InputValue);
        // 2. 计算权重 (双线性插值简化版本)
        TArray<float> Weights = ComputeBilinearWeights(InputValue, NeighborIndices);
        if (NeighborIndices.Num() == 0 || Weights.Num() == 0)
        {
            OutputPose = Samples[0].AnimationPose; // 默认输出第一个样本
            return;
        }
        int32 NumBones = Samples[NeighborIndices[0]].AnimationPose.GetNumBones();
        OutputPose.BoneTransforms.SetNum(NumBones);
        // 3. 按权重插值骨骼姿势
        for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
        {
            FVector FinalLoc = FVector::ZeroVector;
            FQuat FinalRot = FQuat::Identity;
            FVector FinalScale = FVector::ZeroVector;
            bool bFirstRot = true;
            for (int32 i = 0; i < NeighborIndices.Num(); ++i)
            {
                const FPose& SamplePose = Samples[NeighborIndices[i]].AnimationPose;
                const FTransform& BoneTransform = SamplePose.BoneTransforms[BoneIndex];
                float Weight = Weights[i];
 
                FinalLoc += BoneTransform.GetLocation() * Weight;
                FinalScale += BoneTransform.GetScale3D() * Weight;
 
                if (bFirstRot)
                {
                    FinalRot = BoneTransform.GetRotation();
                    bFirstRot = false;
                }
                else
                {
                    FinalRot = FQuat::Slerp(FinalRot, BoneTransform.GetRotation(), Weight).GetNormalized();
                }
            }
            OutputPose.BoneTransforms[BoneIndex].SetLocation(FinalLoc);
            OutputPose.BoneTransforms[BoneIndex].SetRotation(FinalRot);
            OutputPose.BoneTransforms[BoneIndex].SetScale3D(FinalScale);
        }
    }
}



















