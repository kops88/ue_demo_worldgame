// 基础动画姿势（骨骼变换集合）
struct Pose
{
    BoneTransform[] BoneTransforms;
}

// 每个动画层的输入参数，包括Pose和权重
struct LayerInput
{
    Pose InputPose;
    float Weight;

}



class AnimationLayer
{
public:
    string LayerName;
    LayerInput Input;
    Pose OutputPose;

    // 处理函数：根据输入生成新Pose（可覆盖、叠加等）
    virtual Pose Process(LayerInput input)
    {
        // 伪代码：简单示意处理
        // 实际中这里可能包含状态机、BlendSpaces、IK调整等复杂逻辑
        return input.InputPose; 
    }
}

// 管理多层动画，按顺序叠加处理Pose
class AnimationLayerManager
{
public:
    // 按叠加顺序排列
    AnimationLayer[] Layers;
    Pose EvaluateFinalPose(Pose BasePose)
    {
        Pose CurrentPose = BasePose;
        for (AnimationLayer layer : Layers)
        {
            layer.Input.InputPose = CurrentPose;
            Pose LayerPose = layer.Process(layer.Input);
            CurrentPose = BlendPoses(CurrentPose, LayerPose, layer.Input.Weight);
        }
        return CurrentPose;
    }
   
    Pose BlendPoses(Pose A, Pose B, float Weight)
    {
        Pose Result;
        for (int i = 0; i < A.BoneTransforms.length; ++i)
        {
            Result.BoneTransforms[i] = Lerp(A.BoneTransforms[i], B.BoneTransforms[i], Weight);
        }
        return Result;
    }
}



















