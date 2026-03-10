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