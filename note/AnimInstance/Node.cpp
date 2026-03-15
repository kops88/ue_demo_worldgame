// 伪代码结构体表示一个动画姿势（骨骼变换数组）
struct FPose
{
    TArray<FTransform> BoneTransforms; // 每个骨骼的变换数组
    int32 GetNumBones() const { return BoneTransforms.Num(); }
};


// BlendMulti节点伪代码
struct FAnimNode_BlendMulti
{
    // 输入：多组动画姿势和对应权重
    TArray<FPose> InputPoses;
    TArray<float> Weights; 

    // 输出：最终混合后的姿势
    FPose OutputPose;

    void Evaluate()
    {
        if (InputPoses.Num() == 0 || Weights.Num() != InputPoses.Num())
        {
            // 无效输入，输出空姿势或默认姿势
            OutputPose.BoneTransforms.Empty();
            return;
        }
        int32 NumBones = InputPoses[0].GetNumBones();
        OutputPose.BoneTransforms.SetNum(NumBones);
        // 归一化权重，防止权重和为0
        float TotalWeight = 0.f;
        for (float W : Weights)
        {
            TotalWeight += W;
        }
        if (TotalWeight <= 0.f)
        {
            // 直接采用第一个输入姿势输出
            OutputPose = InputPoses[0];
            return;
        }
        // 对每个骨骼进行加权混合
        for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
        {
            FVector FinalLocation = FVector::ZeroVector;
            FQuat FinalRotation = FQuat::Identity;
            FVector FinalScale = FVector::ZeroVector;

            bool bFirstRotation = true;

            // 遍历所有姿势，累加加权的位置、旋转、缩放
            for (int32 PoseIndex = 0; PoseIndex < InputPoses.Num(); ++PoseIndex)
            {
                float NormWeight = Weights[PoseIndex] / TotalWeight;
                const FTransform& CurTransform = InputPoses[PoseIndex].BoneTransforms[BoneIndex];

                // 位置和缩放线性插值累加
                FinalLocation += CurTransform.GetLocation() * NormWeight;
                FinalScale += CurTransform.GetScale3D() * NormWeight;

                // 旋转球面插值累加（基于逐步SLERP平均技巧）
                if (bFirstRotation)
                {
                    FinalRotation = CurTransform.GetRotation();
                    bFirstRotation = false;
                }
                else
                {
                    FinalRotation = FQuat::Slerp(FinalRotation, CurTransform.GetRotation(), NormWeight).GetNormalized();
                }
            }
            // 设置混合结果到输出骨骼姿势
            OutputPose.BoneTransforms[BoneIndex].SetLocation(FinalLocation);
            OutputPose.BoneTransforms[BoneIndex].SetRotation(FinalRotation);
            OutputPose.BoneTransforms[BoneIndex].SetScale3D(FinalScale);
        }
    }




}

struct FAnimNode_ApplyAdditive
{
    FPose BasePose;      // 基准姿势
    FPose AdditivePose;  // 附加姿势
    FPose RefPose;       // 参考姿势
    float Alpha;         // 叠加权重 (0~1)
    FPose OutputPose;    // 输出姿势

    void Evaluate()
    {
        int32 NumBones = BasePose.GetNumBones();
        OutputPose.BoneTransforms.SetNum(NumBones);
        for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
        {
            // 取基础、附加和参考骨骼变换
            const FTransform& BaseT = BasePose.BoneTransforms[BoneIndex];
            const FTransform& AddT = AdditivePose.BoneTransforms[BoneIndex];
            const FTransform& RefT = RefPose.BoneTransforms[BoneIndex];

            // 计算附加变换相对于参考姿势的增量
            FVector DeltaLoc = AddT.GetLocation() - RefT.GetLocation();
            FQuat DeltaRot = AddT.GetRotation() * RefT.GetRotation().Inverse();
            FVector DeltaScale = AddT.GetScale3D() - RefT.GetScale3D();

            // 按Alpha加权叠加增量到基准变换
            FVector BlendedLoc = BaseT.GetLocation() + DeltaLoc * Alpha;
            FQuat BlendedRot = FQuat::Slerp(BaseT.GetRotation(), DeltaRot * BaseT.GetRotation(), Alpha).GetNormalized();
            FVector BlendedScale = BaseT.GetScale3D() + DeltaScale * Alpha;
            // 写入输出姿势
            OutputPose.BoneTransforms[BoneIndex].SetLocation(BlendedLoc);
            OutputPose.BoneTransforms[BoneIndex].SetRotation(BlendedRot);
            OutputPose.BoneTransforms[BoneIndex].SetScale3D(BlendedScale);
        }
    }
}