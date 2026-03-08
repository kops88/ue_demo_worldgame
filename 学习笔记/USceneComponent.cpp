class USceneComponent
{
    /** 变换 */
public:
    FVector Location;
    FRotator Rotation;
    FVector Scale;

    void SetWorldLocation(FVector NewLocation) { Location = NewLocation; }
    FVector GetWorldLocation() const { return Location;}
    void SetWorldRotation(FRotator NewRotation) { Rotation = NewRotation; }
    FRotator GetWorldRotation() const { return Location;}
    void SetWorldScale(FVector NewScale) { Scale = NewScale; }
    FVector GetWorldScale() const { return Scale; }

/** 场景组件可以相互附加（Attach），形成父子层级树。子组件的变换是相对于其父组件的。 */
    public:
    USceneComponent* Parent = nullptr;
    TArray<USceneComponent*> Children;
    FVector RelativeLocation = {0, 0, 0};
    FRotator RelativeRotation = {0, 0, 0};
    FVector RelativeScale = {1, 1, 1};

    FVector WorldLocation = {0, 0, 0};
    FRotator WorldRotation = {0, 0, 0};
    FVector WorldScale = {1, 1, 1};

     void AttachTo(USceneComponent* NewParent)
    {
        if (Parent)
        {
            Parent->Children.Remove(this);
        }
        Parent = NewParent;
        if (Parent)
        {
            Parent->Children.Add(this);
        }
        UpdateWorldTransform();
    }

    void UpdateWorldTransform()
    {
        if (Parent)
        {
            WorldScale = Parent->WorldScale * RelativeScale;
            WorldRotation = Parent->WorldRotation + RelativeRotation;     // 简化旋转合成
            WorldLocation = Parent->WorldLocation + Parent->WorldRotation.RotateVector(RelativeLocation * Parent->WorldScale);
        }
        else
        {
            WorldLocation = RelativeLocation;
            WorldRotation = RelativeRotation;
            WorldScale = RelativeScale;
        }
 
        for (USceneComponent* Child : Children)
        {
            Child->UpdateWorldTransform();
        }
    }
}



	class AActor
{
public:
    // RootComponent存储Actor的变换（位置、旋转、缩放）
    USceneComponent* RootComponent;
    // 包含的组件列表，支持功能扩展（渲染、碰撞等）
    TArray<UActorComponent*> Components;

     AActor()
    {
        // 创建根组件，Actor变换依托此组件
        RootComponent = new USceneComponent();
        Components.Add(RootComponent);
    }
}