
	UCLASS()
class MYGAME_API UMyGameInstanceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override
    {
         Counter = 0;
         TrackedActors.Empty();
    }

    virtual void Deinitialize() override
    {
        TrackedActors.Empty();
    }


	private:
    // 核心数据结构：计数器
    int32 Counter;
    TArray<TWeakObjectPtr<AActor>> TrackedActors;
}