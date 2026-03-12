class UActorComponent
{
    // 纯功能扩展: 基础组件类，功能扩展用，非变换相关
public:
    /** 生命周期管理 */
    virtual void Tick(float DeltaTime) {}
    virtual void Initialize() {}
	virtual void TickComponent(float DeltaTime) { /* 每帧更新逻辑 */ }
    virtual void EndPlay() { /* 结束时清理 */ }
    /** 激活 停用 */
    void SetActive(bool bNewActive) { bIsActive = bNewActive; }
    bool IsActive() const { return bIsActive; }
    /** 所有权 */
    bool bIsActive = true;
    AActor* Owner = nullptr;
    AActor* GetOwner() const { return Owner; }
    void SetOwner(AActor* InOwner) { Owner = InOwner; }

    /** tick 控制 */
    FActorComponentTickFunction PrimaryComponentTick;

    /** 事件委托 */
public:
    using EventDelegate = std::function<void(UActorComponent*)>;
    std::vector<EventDelegate> OnActivatedListeners;
    void SetActive(bool bNewActive) { 
        if (bIsActive != bNewActive)
        {
            bIsActive = bNewActive;
            if (bIsActive)
            {
                BroadcastOnActivated(); //激活事件
            }
        }
        bIsActive = bNewActive; 
    }
    /** 绑定激活事件监听函数 */
    void RegisterOnActivated(EventDelegate Listener)
    {
        OnActivatedListeners.push_back(Listener);
    }
    void BroadcastOnActivated()
    {
        for (auto& Listener : OnActivatedListeners)
        {
            Listener(this);
        }
    }

protected:
    

};