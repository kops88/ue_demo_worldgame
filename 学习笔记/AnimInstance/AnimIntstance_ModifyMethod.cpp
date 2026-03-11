/*
 * @Author: kops88c 3036435162@qq.com
 * @Date: 2026-03-11 19:14:51
 * @LastEditors: kops88c 3036435162@qq.com
 * @LastEditTime: 2026-03-11 21:05:47
 * @FilePath: \ue_demo_worldgame\学习笔记\AnimInstance\AnimIntstance_ModifyMethod.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
	enum ModifyMethod
{
    Add,
    Scale,
    Blend,
    Remap //重映射
}

struct ModifyCurveData
{
    string CurveName;
    float CurrentValue;  // 当前运行时曲线值
    ModifyMethod Method; // 应用的修改方法
    float Value;         // 用于Add/Scale/Blend的新值或Remap的输入参数
    float Alpha;         // Blend或WeightedMovingAverage的权重（0-1）
    float RemapInMin;    // Remap输入区间起点
    float RemapInMax;    // Remap输入区间终点
    float RemapOutMin;   // Remap输出区间起点
    float RemapOutMax;   // Remap输出区间终点
}


class ModifyCurveOperator
{
     float ApplyModification(ModifyCurveData data)
     {
        switch(data.Method)
        {
            case Add:
                return data.CurrentValue + data.Value;

            case Scale:
                return data.CurrentValue * data.Value;
 
            case Blend:
            /* 输入*(1-alpha) + modifyCurve*alpha, 0 完全使用输入, 1完全使用modifyCurve */
                return data.CurrentValue * (1 - data.Alpha) + data.Value * data.Alpha;

            case Remap:
                float clamped = Clamp(data.CurrentValue, data.RemapInMin, data.RemapInMax); // clamped 在currentvalue 和 min max 三选一
                float normalized = (clamped - data.RemapInMin) / (data.RemapInMax - data.RemapInMin);
                return data.RemapOutMin + normalized * (data.RemapOutMax - data.RemapOutMin);

            default:
                return data.CurrentValue;
        }
     }
}


class BlendPoseNode
{
    // 多个输入动画姿势
    InputPoses: List<AnimPose>

    // 切换到当前pose的过度时间
    BlendTimes: List<float>
    
    /** 只输出当前激活的pose, 如果切换enum, 会平滑过度到目标pose, 而非直接切换 */
    activeEnumValue: enum

}
















