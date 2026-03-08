#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Yy/enum/yyCharacterStructLibrary.h"
#include "yyBaseCharacter.generated.h"


/* 站蹲 的步伐模式速度*/
USTRUCT(BlueprintType)
struct FyyMovementStanceSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	FyyMovementSettings Standing;
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	FyyMovementSettings Crouching;
};

/* 角色朝向的 站蹲的步伐模式速度*/
USTRUCT(BlueprintType)
struct FyyMovementStateSettings : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	FyyMovementStanceSettings VelocityDirection;
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	FyyMovementStanceSettings LookingDirection;
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	FyyMovementStanceSettings Aiming;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRagdollStateChangedSignature, bool, bRagdollState);


UCLASS()
class WORLDGAME_API AyyBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AyyBaseCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SetRotationMode(EyyRotationMode NewRotationMode, bool bForce = false);
	UFUNCTION(BlueprintCallable, Category = "Movement System")
	EyyGait GetAllowedGait() const;
	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	bool CanSprint() const;
	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	void SetCameraBehavior(UyyPlayerCameraBehavior* CamBeh) { CameraBehavior = CamBeh; };
	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	virtual FVector GetFirstPersonCameraTarget();
	/* 获取摄像机参数: 视角, 肩位 */
	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	void GetCameraParameters(float& TPFOVOut, float& FPFOVOut, bool& bRightShoulderOut) const;
	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	virtual ECollisionChannel GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius);


	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetGait(EyyGait NewGait, bool bForce = false);
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetStance(EyyStance NewStance, bool bForce = false);
	
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetViewMode(EyyViewMode NewViewMode, bool bForce = false);
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetOverlayState(EyyOverlayState NewState, bool bForce = false);
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetMovementState(EyyMovementState NewState, bool bForce = false);
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetMovementAction(EyyMovementAction NewAction, bool bForce = false);
	UFUNCTION(BlueprintCallable, Category = "Movement System")
	FyyMovementSettings GetTargetMovementSettings() const;
	UFUNCTION(BlueprintCallable, Category = "Movement System")
	EyyGait GetActualGait(EyyGait AllowedGait) const;
	UFUNCTION(BlueprintCallable, Category = "ALS|Utility")
	float GetAnimCurveValue(FName CurveName) const;
	UFUNCTION(BlueprintCallable, Category = "ALS|Rotation System")
	void SetActorLocationAndTargetRotation(FVector NewLocation, FRotator NewRotation);
	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EyyMovementState GetMovementState() const { return MovementState; }
	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EyyMovementAction GetMovementAction() const { return MovementAction; }
	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	virtual FTransform GetThirdPersonPivotTarget();

	UFUNCTION(BlueprintGetter, Category = "ALS|Camera System")
	bool IsRightShoulder() const { return bRightShoulder; }
	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EyyGait GetGait() const { return Gait; }
	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EyyRotationMode GetRotationMode() const { return RotationMode; }
	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EyyStance GetStance() const { return Stance; }
	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EyyViewMode GetViewMode() const { return ViewMode; }

	
	/** Ragdolling*/
	// UFUNCTION(BlueprintCallable, Category = "Character States")
	// void ReplicatedRagdollStart();
	// UFUNCTION(BlueprintCallable, Category = "Character States")
	// void Multicast_RagdollStart();
	UFUNCTION(BlueprintCallable, Category = "Ragdoll System")
	virtual void RagdollStart();
	UPROPERTY(BlueprintAssignable, Category = "ALS|Input")
	FRagdollStateChangedSignature RagdollStateChangedDelegate;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Ragdoll System")
	FVector TargetRagdollLocation = FVector::ZeroVector;
	bool bPreRagdollURO = false;

	/** Input */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void ForwardMovementAction(float Value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void RightMovementAction(float Value);
	
protected:
	void SetMovementModel();
	void ForceUpdateCharacterState();
	
	virtual void OnGaitChanged(EyyGait PreviousGait);
	virtual void OnStanceChanged(EyyStance PreviousStance);
	virtual void OnRotationModeChanged();
	virtual void OnViewModeChanged();
	virtual void OnOverlayStateChanged(EyyOverlayState PreviousState);
	virtual void OnMovementStateChanged(EyyMovementState PreviousState);
	virtual void OnMovementActionChanged(EyyMovementAction PreviousAction);
	
	void SetEssentialValues(float DeltaTime);
	void UpdateCharacterMovement();
	void UpdateGroundedRotation(float DeltaTime);
	float CalculateGroundedRotationRate() const;
	void SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed, float DeltaTime);
	void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime);
	void UpdateInAirRotation(float DeltaTime);
	void RagdollUpdate(float DeltaTime);
	void SetActorLocationDuringRagdoll(float DeltaTime);
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement System")
	FDataTableRowHandle MovementModel;
	UPROPERTY(BlueprintReadOnly, Category = "Movement System")
	FyyMovementStateSettings MovementData;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Input")
	EyyRotationMode DesiredRotationMode = EyyRotationMode::LookingDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EyyGait DesiredGait = EyyGait::Running;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EyyStance DesiredStance = EyyStance::Standing;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Values")
	EyyOverlayState OverlayState = EyyOverlayState::Default;
	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EyyMovementAction MovementAction = EyyMovementAction::None;
	UPROPERTY(BlueprintReadOnly, Category = "Rotation System")
	FRotator InAirRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadOnly, Category = "Rotation System")
	FRotator TargetRotation = FRotator::ZeroRotator;
	/* 上一次速度的旋转, 有一个角度为0*/
	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
	FRotator LastVelocityRotation;
	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
	FRotator LastMovementInputRotation;
	UPROPERTY()
	TObjectPtr<class UyyCharacterMovementComponent> MyCharacterMovementComponent;
	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	FVector LastRagdollVelocity = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EyyGait Gait = EyyGait::Walking;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Values")
	EyyStance Stance = EyyStance::Standing;
	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EyyRotationMode RotationMode = EyyRotationMode::LookingDirection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Values")
	EyyViewMode ViewMode = EyyViewMode::ThirdPerson;
	/* 持久性运动状态 */
	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EyyMovementState MovementState = EyyMovementState::None;
	/* 前一次 持久性运动状态 */
	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EyyMovementState PrevMovementState = EyyMovementState::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Skeletal Mesh")
	TObjectPtr<USkeletalMesh> VisibleMesh = nullptr;
	/* 第三人称视角 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	float ThirdPersonFOV = 90.0f;
	/* 第一人称视角 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	float FirstPersonFOV = 90.0f;
	/* 摄像机是否在右肩 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	bool bRightShoulder = false;
	
	/* camera */
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UyyPlayerCameraBehavior> CameraBehavior;
	
	/* 瞄准目标朝向 */
	FRotator AimingRotation = FRotator::ZeroRotator;
	FVector PreviousVelocity = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ALS|Essential Information")
	FVector ReplicatedCurrentAcceleration = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float EasedMaxAcceleration = 0.0f;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ALS|Essential Information")
	FRotator ReplicatedControlRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
	float Speed = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
	bool bIsMoving = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Essential Information")
	float AimYawRate = 0.0f;
	float PreviousAimYaw = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	bool bHasMovementInput = false;
	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float MovementInputAmount = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Ragdoll System")
	bool bReversedPelvis = false;
	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	bool bRagdollFaceUp = false;
	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	bool bRagdollOnGround = false;
	bool bEnableNetworkOptimizations = false;

	
	
};














