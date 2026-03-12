// 定时器句柄 —— 用于标识和操控一个定时器
struct FTimerHandle {
    // 唯一ID，低位存索引，高位存序列号
    uint64 Handle;  
}

struct FTimerData {
    // 回调函数（可绑定成员函数/Lambda）
    TimerDelegate  TimerDelegate;  
    // 到期的绝对时间（WorldTime + Rate）
    double ExpireTime;
    // 触发间隔（<=0 表示一次性）
    float Rate;  
    // 是否循环
    bool bLoop;
    // 是否需要回调
    bool bRequiresDelegate;
     // 状态：Active / Paused / Executing / Pending
    ETimerStatus Status;
}

// TimerManager 内部核心容器
class FTimerManager {
    // 最小堆：按 ExpireTime 排序，堆顶是最快到期的定时器
    TArray<FTimerData*>  ActiveTimerHeap;
    // 暂停中的定时器列表
    TArray<FTimerData>   PausedTimerList;
    // 等待加入堆的定时器（当前帧新增，下帧生效）
    TArray<FTimerData>   PendingTimerList;
    // Handle -> TimerData 的映射，用于 O(1) 查找
    TMap<FTimerHandle, FTimerData*> TimerMap;
    // 当前世界时间（每帧 Tick 累加）
    double  InternalTime;   


    void Tick(float DeltaTime) 
    {

        InternalTime += DeltaTime;

        // 1. 将 Pending 列表中的新定时器插入最小堆
        foreach timer in PendingTimerList:
        {
            InsertHeap(ActiveTimerHeap, timer);  // O(log N)
        }
        PendingTimerList.clear()

        // 2. 循环触发所有已到期的定时器（堆顶 ExpireTime <= InternalTime）
        while ActiveTimerHeap.Top().ExpireTime <= InternalTime:
        {
            // O(log N) 弹出堆顶
            timer = ActiveTimerHeap.PopTop()    
            timer.Status = Executing
            // 执行回调
            timer.TimerDelegate.Execute()

            if timer.bLoop:
            {
                // 循环定时器：重新计算下次到期时间，重新入堆
                timer.ExpireTime += timer.Rate
                InsertHeap(ActiveTimerHeap, timer)
            }
            else:
            {
                // 一次性定时器：从 TimerMap 移除
                TimerMap.Remove(timer.Handle)
            }
        }
    }


    // ① 设置定时器（最常用）
    // Rate=0.1f, bLoop=false → 0.1秒后触发一次
    SetTimer(
        FTimerHandle& OutHandle,    // 输出句柄，用于后续操控
        UserClass* Object,          // 回调对象
        Callback,                   // 成员函数指针
        float Rate,                 // 延迟/间隔（秒）
        bool bLoop,                 // 是否循环
        float FirstDelay = -1.0f    // 首次触发延迟（-1 表示用 Rate）
    ) {
        // 若 OutHandle 已有定时器，先清除旧的
        if OutHandle.IsValid()
        {
            ClearTimer(OutHandle)
        }

        FTimerData newTimer;
        newTimer.TimerDelegate = BindDelegate(Object, Callback);
        newTimer.Rate = Rate;
        newTimer.bLoop = bLoop;
        newTimer.ExpireTime = InternalTime + (FirstDelay >= 0 ? FirstDelay : Rate);
        newTimer.Status = Pending;

        OutHandle = GenerateHandle();
        TimerMap[OutHandle] = newTimer;
        // 下帧才进堆
        PendingTimerList.Add(newTimer)  
    }

    ClearTimer(FTimerHandle& Handle) {
        // 1. 找到 Handle 对应的 FTimerData，将其从容器中移除
        //    （从 ActiveTimerHeap / PausedTimerList / PendingTimerList 中标记或删除）
        if TimerMap.Contains(Handle)
        {
            timer = TimerMap[Handle];
            // 标记为无效
            timer.Status = Cleared;      
            TimerMap.Remove(Handle);
            Handle.Handle = 0;
        }
    }

}





















