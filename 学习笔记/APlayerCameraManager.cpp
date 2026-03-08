struct FMinimalViewInfo
{
    FVector Location;
    FRotator Rotation;
    float FOV;
    // 省略其他参数（如投影模式等）
}

struct FViewTarget
{
    AActor* TargetActor = nullptr;
    // Actor上的摄像机组件，如有
    UCameraComponent* CameraComponent = nullptr; 
    FMinimalViewInfo POV; 
}


class APlayerCameraManager : public AActor
{
    /** 视角管理 */
public:
    // 当前跟随的Actor
    FViewTarget CurrentViewTarget;
    // 当前帧计算出的最终摄像机视图
    FMinimalViewInfo CameraCache;

    float DefaultFOV = 90.f;

    // 设置视图目标Actor，并查找其摄像机组件（如有）
    void SetViewTarget(AActor* NewTarget)
    {
        CurrentViewTarget.TargetActor = NewTarget;
        CurrentViewTarget.CameraComponent = FindCameraComponent(NewTarget);
    }

    UCameraComponent* FindCameraComponent(AActor* Actor)
    {
        if (!Actor) return nullptr;
        for (UActorComponent* Comp : Actor->Components)
        {
            if (Comp is UCameraComponent)
                return static_cast<UCameraComponent*>(Comp);
        }
        return nullptr;
    }

    // 每帧调用，计算并缓存最终摄像机视图
    void CalcCamera(float DeltaTime)
    {
        UpdateViewTarget();
        CameraCache = CurrentViewTarget.POV;
    }

    // 根据当前ViewTarget更新POV信息
    void UpdateViewTarget()
    {
        if (CurrentViewTarget.CameraComponent)
        {
            CurrentViewTarget.POV = CurrentViewTarget.CameraComponent->GetCameraView();
        }
        else if (CurrentViewTarget.TargetActor)
        {
            // 无摄像机组件时，用Actor位置和旋转作为视图
            CurrentViewTarget.POV.Location = CurrentViewTarget.TargetActor->GetActorLocation();
            CurrentViewTarget.POV.Rotation = CurrentViewTarget.TargetActor->GetActorRotation();
            CurrentViewTarget.POV.FOV = DefaultFOV;
        }
        else 
        {
            // 无目标时，设置默认视图
            CurrentViewTarget.POV.Location = FVector{0,0,0};
            CurrentViewTarget.POV.Rotation = FRotator{0,0,0};
            CurrentViewTarget.POV.FOV = DefaultFOV;
        }
    }
    FMinimalViewInfo GetCameraViewPoint() const
    {
        return CameraCache;
    }

}