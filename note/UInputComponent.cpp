enum class EInputEvent
{
    Pressed,
    Released,
    Held
}

/** 动作事件: 名字, 触发类型, 事件 */
struct FActionBinding
{
    std::string ActionName;
    EInputEvent EventType;
    std::function<void()> Callback;
}

// 轴绑定：对应Axis名称和带参数的回调函数
struct FAxisBinding
{
    std::string AxisName;
    std::function<void(float)> Callback;
};

	// 按键映射：硬件按键映射到游戏Action或Axis，包含轴缩放因子
struct FKeyMapping
{
    std::string HardwareKey;      // 硬件输入名称，如"W", "SpaceBar"
    std::string MappedName;       // 对应的Action或Axis名
    bool bIsAxis;                 // true表示轴映射，false表示动作映射
    float AxisScale;              // 轴输入缩放，动作映射忽略
}

class UInputComponent
{
public:
    // 绑定的动作事件列表
    std::vector<FActionBinding> ActionBindings;
    // 绑定的轴事件列表
    std::vector<FAxisBinding> AxisBindings;
    // 硬件按键映射表：按键名 -> 映射的动作/轴项
    std::unordered_map<std::string, std::vector<FKeyMapping>> KeyMappings;
    // 记录上一帧键位状态，用于判断Pressed/Released事件
    std::unordered_map<std::string, bool> PreviousKeyStates;

    // 绑定动作事件回调
    void BindAction(const std::string& ActionName, EInputEvent EventType, std::function<void()> Callback)
    {
        ActionBindings.push_back({ActionName, EventType, Callback});
    }

    // 绑定轴事件回调
    void BindAxis(const std::string& AxisName, std::function<void(float)> Callback)
    {
        AxisBindings.push_back({AxisName, Callback});
    }

    // 注册硬件按键映射
    void RegisterKeyMapping(const FKeyMapping& Mapping)
    {
        KeyMappings[Mapping.HardwareKey].push_back(Mapping);
    }

    // 输入系统传入本帧硬件键位和轴状态，进行事件分发
    void ProcessInput(
        const std::unordered_map<std::string, bool>& CurrentKeyStates,
        const std::unordered_map<std::string, float>& CurrentAxisStates)
    {
        for (const auto& KeyPair : KeyMappings)
        {
            const std::string& HardwareKey = KeyPair.first;
            const auto& Mappings = KeyPair.second;
            bool PrevPressed = PreviousKeyStates[HardwareKey];
            bool CurrPressed = false;
            auto KeyStateIt = CurrentKeyStates.find(HardwareKey);
            if (KeyStateIt != CurrentKeyStates.end())
                CurrPressed = KeyStateIt->second;
            for (const FKeyMapping& Mapping : Mappings)
            {
                if (!Mapping.bIsAxis)
                {
                     for (const auto& Binding : ActionBindings)
                    {
                        if (Binding.ActionName == Mapping.MappedName)
                        {
                            if (Binding.EventType == EInputEvent::Pressed && !PrevPressed && CurrPressed)
                                Binding.Callback();
 
                            else if (Binding.EventType == EInputEvent::Released && PrevPressed && !CurrPressed)
                                Binding.Callback();
 
                            else if (Binding.EventType == EInputEvent::Held && CurrPressed)
                                Binding.Callback();
                        }
                    }
                }
                else
                {
                    // 轴事件：查找对应轴输入，乘以缩放调用回调
                    auto AxisIt = CurrentAxisStates.find(HardwareKey);
                    if (AxisIt != CurrentAxisStates.end())
                    {
                        float Value = AxisIt->second * Mapping.AxisScale;
                        for (const auto& Binding : AxisBindings)
                        {
                            if (Binding.AxisName == Mapping.MappedName)
                            {
                                Binding.Callback(Value);
                            }
                        }
                    }
                }
            }
            PreviousKeyStates[HardwareKey] = CurrPressed; // 记录当前状态供下一帧使用
        }
    }







}