
// 动画插槽描述结构，定义插槽名称及影响骨骼和混合权重
struct FAnimSlot
{
    FName SlotName;                      // 插槽名称，如 "UpperBody", "FullBody"
    TArray<int32> AffectedBoneIndices;  // 受影响的骨骼索引列表
    float BlendWeight = 1.0f;            // 混合权重（0~1）

    FAnimSlot(){}
}

	// 管理动画蒙太奇与插槽，保存动画段及播放状态
struct FAnimMontageSectionInfo
{
    UAnimMontage* Montage;
    FName SlotName;
    float StartTime;
    float EndTime;

    FAnimMontageSectionInfo(){}
}



	// 动画插槽管理组件，负责插槽动画播放控制和分层混合
class FAnimationSlotComponent
{
	public:
    TArray<FAnimSlot> Slots; // 管理所有插槽配置

    // 播放蒙太奇到指定插槽，设置混合时间
    void PlayMontageInSlot(UAnimInstance* AnimInstance, UAnimMontage* Montage, FName SlotName, float BlendInTime, float BlendOutTime)
    {
        if (!AnimInstance || !Montage) return;
        // 为蒙太奇设置插槽名（简化示意，实际需找到对应插槽Track）
        Montage->SlotAnimTracks[0].SlotName = SlotName;
        // 播放蒙太奇（1.0f播放速度，自动融合）
        AnimInstance->Montage_Play(Montage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
         // 设置融合时间（伪代码，实际API有所不同）
        Montage->BlendIn.SetBlendTime(BlendInTime);
        Montage->BlendOut.SetBlendTime(BlendOutTime);
    }



}


















