
struct AnimationCurve
{
    string CurveName;
    float CurrentValue;
    AnimationCurve(string name)
    {
        CurveName = name;
        CurrentValue = 0.0f;
    }
}


class AnimInstance
{
private: 
    Map<string, AnimationCurve> Curves;

public:
    // 添加或更新曲线。如果曲线不存在则创建
    void AddOrUpdateCurve(string curveName, float value)
    {
        if (Curves.Contains(curveName))
        {
            Curves[curveName].CurrentValue = value;
        }
        else
        {
            Curves[curveName] = AnimationCurve(curveName);
            Curves[curveName].CurrentValue = value;
        }
    }

    // 对指定曲线进行混合操作 (Blend)
    // alpha范围 [0,1]，alpha越大新值权重越高
    void BlendCurve(string curveName, float newValue, float alpha)
    {
        float current = GetCurveValue(curveName);
        float blended = current * (1.0f - alpha) + newValue * alpha;
        AddOrUpdateCurve(curveName, blended);
    }
}

struct CurveKey
{
    float Time;
    float Value;
}

class AnimationCurve
{
    List<CurveKey> keys;

public:
    float SampleAtTime(float currentTime)
    {
        if (Keys.IsEmpty())
            return 0.0f;

        if (currentTime <= Keys[0].Time)
            return Keys[0].Value;

        if (currentTime >= Keys.Last().Time)
            return Keys.Last().Value;

        // 查找currentTime所在的区间
        for (int i = 0; i < Keys.Count - 1; i++)
        {
            CurveKey keyA = Keys[i];
            CurveKey keyB = Keys[i+1];
 
            if (currentTime >= keyA.Time && currentTime <= keyB.Time)
            {
                float alpha = (currentTime - keyA.Time) / (keyB.Time - keyA.Time);
                // 线性插值
                return keyA.Value * (1 - alpha) + keyB.Value * alpha;
            }
        }
        return 0.0f; // 不应该达到这里 
    }

}