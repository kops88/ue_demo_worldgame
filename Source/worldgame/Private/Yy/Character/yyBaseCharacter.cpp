#include "Yy/Character/yyBaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Yy/Character/yyCharacterMovementComponent.h"
#include "Yy/Character/yyPlayerCameraBehavior.h"


const FName NAME_Pelvis(TEXT("Pelvis"));
const FName NAME_YawOffset(TEXT("YawOffset"));
const FName NAME_RotationAmount(TEXT("RotationAmount"));
const FName NAME_root(TEXT("root"));
const FName NAME_FP_Camera(TEXT("FP_Camera"));



// Sets default values
AyyBaseCharacter::AyyBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UyyCharacterMovementComponent>(CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
}
void AyyBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MyCharacterMovementComponent = Cast<UyyCharacterMovementComponent>(Super::GetMovementComponent());
}
/* 初始化状态标志, 角色旋转, 运动速度等*/
void AyyBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->AddTickPrerequisiteActor(this);
	SetMovementModel();
	ForceUpdateCharacterState();
	
	if (Stance == EyyStance::Standing)
	{
		UnCrouch();
	}
	else if (Stance == EyyStance::Crouching)
	{
		Crouch();
	}
	TargetRotation = GetActorRotation();
	LastVelocityRotation = TargetRotation;
	LastMovementInputRotation = TargetRotation;
	MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
}



/*
 *设置 持久运动状态标志为 Ragdoll
 *让角色"瘫倒"，像一个布娃娃一样受物理控制倒下去
 * 设置Mesh相关碰撞
 
 */
void AyyBaseCharacter::RagdollStart()
{
	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(true);
	}
	TargetRagdollLocation = GetMesh()->GetSocketLocation(NAME_Pelvis);
	bPreRagdollURO = GetMesh()->bEnableUpdateRateOptimizations;
	GetMesh()->bEnableUpdateRateOptimizations = false;
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	SetMovementState(EyyMovementState::Ragdoll);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(NAME_Pelvis, true, true);
	
	if (GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.2f);
	}
	GetMesh()->bOnlyAllowAutonomousTickPose = true;
	SetReplicateMovement(false);
}


void AyyBaseCharacter::ForwardMovementAction_Implementation(float Value)
{
	if (MovementState == EyyMovementState::Grounded || MovementState == EyyMovementState::InAir)
	{
		const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
		AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Value);
	}
}

void AyyBaseCharacter::RightMovementAction_Implementation(float Value)
{
	if (MovementState == EyyMovementState::Grounded || MovementState == EyyMovementState::InAir)
	{
		const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
		AddMovementInput(UKismetMathLibrary::GetRightVector(DirRotator), Value);
	}
}

void AyyBaseCharacter::SetMovementModel()
{
	const FString ContextString = GetFullName();
	FyyMovementStateSettings* OutRow = MovementModel.DataTable->FindRow<FyyMovementStateSettings>(MovementModel.RowName, ContextString);
	check(OutRow);
	MovementData = *OutRow;
}


/* 更新运动相关的角色状态标志 */
void AyyBaseCharacter::ForceUpdateCharacterState()
{
	SetGait(DesiredGait, true);
	SetStance(DesiredStance, true);
	SetRotationMode(DesiredRotationMode, true);
	SetViewMode(ViewMode, true);
	SetOverlayState(OverlayState, true);
	SetMovementState(MovementState, true);
	SetMovementAction(MovementAction, true);
}
/* 更改摄像机状态Gait*/
void AyyBaseCharacter::OnGaitChanged(EyyGait PreviousGait)
{
	if (CameraBehavior)
	{
		CameraBehavior->Gait = Gait;
	}
}

void AyyBaseCharacter::OnStanceChanged(EyyStance PreviousStance)
{
	if (CameraBehavior)
	{
		CameraBehavior->Stance = Stance;
	}
}

/* 
 * 根据RotationMode设置camera角色朝向模式的状态
 * 如果是 角色不跟随镜头方向 且 第一人称, 则冲突, 自动更换到第三人称
 */
void AyyBaseCharacter::OnRotationModeChanged()
{
	if (RotationMode == EyyRotationMode::VelocityDirection && ViewMode == EyyViewMode::FirstPerson)
	{
		SetViewMode(EyyViewMode::ThirdPerson);
	}
	if (CameraBehavior)
	{
		CameraBehavior->SetRotationMode(RotationMode);
	}
}

/* 
 * 根据状态ViewMode RotationMode 设置角色朝向模式, 和摄像机一\三人称
 * ViewMode为三人称, 根据DesiredRotationMode设置角色朝向模式
 * ViewMode为一人称, 仅仅设置为角色跟随镜头方向
 * 并根据ViewMode设置摄像机模式
 */
void AyyBaseCharacter::OnViewModeChanged()
{
	if (ViewMode == EyyViewMode::ThirdPerson)
	{
		if (RotationMode == EyyRotationMode::VelocityDirection || RotationMode == EyyRotationMode::LookingDirection)
		{
			SetRotationMode(DesiredRotationMode);
		}
	}
	else if (ViewMode == EyyViewMode::FirstPerson && RotationMode == EyyRotationMode::VelocityDirection)
	{
		SetRotationMode(EyyRotationMode::LookingDirection);
	}
	if (CameraBehavior)
	{
		CameraBehavior->ViewMode = ViewMode;
	}
}

/* 源码空的 */
void AyyBaseCharacter::OnOverlayStateChanged(EyyOverlayState PreviousState)
{
}

/*
 *更新 持久运动状态: 地上\坠落\攀爬\失去意识的状态标志
 * 运动组件的Crouch
 */
void AyyBaseCharacter::OnMovementStateChanged(EyyMovementState PreviousState)
{
	if (MovementState == EyyMovementState::InAir)
	{
		if (MovementAction == EyyMovementAction::None)
		{
			InAirRotation = GetActorRotation();
			if (Stance == EyyStance::Crouching)
			{
				UnCrouch();
			}
		}
	}
	    /* 在空中触发翻滚， 进入布娃娃？ */
		// else if (MovementAction == EyyMovementAction::Rolling)
		// {
		// 	ReplicatedRagdollStart();
		// }
	if (CameraBehavior)
	{
		CameraBehavior->MovementState = MovementState;
	}
}

/* 
 * 更新CameraBehavior的MovementAction状态标志
 * 更新MovementComponent 运动动作发生变化, 仅限Crouch的更新, 
 */
void AyyBaseCharacter::OnMovementActionChanged(EyyMovementAction PreviousAction)
{
	if (MovementAction == EyyMovementAction::Rolling)
	{
		Crouch();
	}
	
	if (PreviousAction == EyyMovementAction::Rolling)
	{
		if (DesiredStance == EyyStance::Standing)
		{
			UnCrouch();
		}
		else if (DesiredStance == EyyStance::Crouching)
		{
			Crouch();
		}
	}
	if (CameraBehavior)
	{
		CameraBehavior->MovementAction = MovementAction;
	}
}

/* 设置速度\是否移动的状态信息*/
void AyyBaseCharacter::SetEssentialValues(float DeltaTime)
{
	if (GetLocalRole() != ROLE_SimulatedProxy)
	{
		ReplicatedCurrentAcceleration = GetCharacterMovement()->GetCurrentAcceleration();
		ReplicatedControlRotation = GetControlRotation();
		EasedMaxAcceleration = GetCharacterMovement()->GetMaxAcceleration();
	}
	else
	{
		EasedMaxAcceleration = GetCharacterMovement()->GetMaxAcceleration() != 0
								   ? GetCharacterMovement()->GetMaxAcceleration()
								   : EasedMaxAcceleration / 2;
	}
	AimingRotation = FMath::RInterpTo(AimingRotation, ReplicatedControlRotation, DeltaTime, 30);
	const FVector CurrentVel = GetVelocity();
	Speed = CurrentVel.Size2D();
	bIsMoving = Speed > 1.0f;
	if (bIsMoving)
	{
		LastVelocityRotation = CurrentVel.ToOrientationRotator();
	}
	MovementInputAmount = ReplicatedCurrentAcceleration.Size() / EasedMaxAcceleration;
	bHasMovementInput = MovementInputAmount > 0.0f;
	if (bHasMovementInput)
	{
		
		LastMovementInputRotation = ReplicatedCurrentAcceleration.ToOrientationRotator();
	}	
	AimYawRate = FMath::Abs((AimingRotation.Yaw - PreviousAimYaw) / DeltaTime);
}


void AyyBaseCharacter::UpdateCharacterMovement()
{
	const EyyGait AllowedGait = GetAllowedGait();
	const EyyGait ActualGait = GetActualGait(AllowedGait);
	if (ActualGait != Gait)
	{
		SetGait(ActualGait);
	}
	MyCharacterMovementComponent->SetAllowedGait(AllowedGait);
}

/* 帧更新地面状态的角色旋转值 */
void AyyBaseCharacter::UpdateGroundedRotation(float DeltaTime)
{
	if (MovementAction == EyyMovementAction::None)
	{
		const bool bCanUpdateMovingRot = ((bIsMoving && bHasMovementInput) || Speed > 150.0f) && !HasAnyRootMotion();
		if (bCanUpdateMovingRot)
		{
			const float GroundedRotationRate = CalculateGroundedRotationRate();
			if (RotationMode == EyyRotationMode::VelocityDirection)
			{
				SmoothCharacterRotation({0.0f, LastVelocityRotation.Yaw, 0.0f}, 800.0f, GroundedRotationRate,
										DeltaTime);
			}
			else if (RotationMode == EyyRotationMode::LookingDirection)
			{
				float YawValue;
				if (Gait == EyyGait::Sprinting)
				{
					YawValue = LastVelocityRotation.Yaw;
				}		
				else
				{
					// Walking or Running..
					const float YawOffsetCurveVal = GetAnimCurveValue(NAME_YawOffset);
					YawValue = AimingRotation.Yaw + YawOffsetCurveVal;
				}
				SmoothCharacterRotation({0.0f, YawValue, 0.0f}, 500.0f, GroundedRotationRate, DeltaTime);
			}
			else if (RotationMode == EyyRotationMode::Aiming)
			{
				const float ControlYaw = AimingRotation.Yaw;
				SmoothCharacterRotation({0.0f, ControlYaw, 0.0f}, 1000.0f, 20.0f, DeltaTime);
			}
		}
		else
		{
			if ((ViewMode == EyyViewMode::ThirdPerson && RotationMode == EyyRotationMode::Aiming) ||
	ViewMode == EyyViewMode::FirstPerson)
			{
				LimitRotation(-100.0f, 100.0f, 20.0f, DeltaTime);
			}
			const float RotAmountCurve = GetAnimCurveValue(NAME_RotationAmount);
			if (FMath::Abs(RotAmountCurve) > 0.001f)
			{
				if (GetLocalRole() == ROLE_AutonomousProxy)
				{
					TargetRotation.Yaw = UKismetMathLibrary::NormalizeAxis(
						TargetRotation.Yaw + (RotAmountCurve * (DeltaTime / (1.0f / 30.0f))));
					SetActorRotation(TargetRotation);
				}
				else
				{
					AddActorWorldRotation({0, RotAmountCurve * (DeltaTime / (1.0f / 30.0f)), 0});
				}
				TargetRotation = GetActorRotation();
			}
		}
	}
	else if (MovementAction == EyyMovementAction::Rolling)
	{
		// Rolling Rotation (Not allowed on networked games)
		if (!bEnableNetworkOptimizations && bHasMovementInput)
		{
			SmoothCharacterRotation({0.0f, LastMovementInputRotation.Yaw, 0.0f}, 0.0f, 2.0f, DeltaTime);
		}
	}	
	// Other actions are ignored...
}

/* 根据UCMC的speed限制(计算)AImYaw瞄准水平旋转速度 */
float AyyBaseCharacter::CalculateGroundedRotationRate() const
{
	const float MappedSpeedVal = MyCharacterMovementComponent->GetMappedSpeed();
	const float CurveVal =
		MyCharacterMovementComponent->CurrentMovementSettings.RotationRateCurve->GetFloatValue(MappedSpeedVal);
	const float ClampedAimYawRate = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 300.0f}, {1.0f, 3.0f}, AimYawRate);
	return CurveVal * ClampedAimYawRate;
}

/* 设置角色的平滑过度旋转 */
void AyyBaseCharacter::SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed,
	float DeltaTime)
{
	TargetRotation =
		FMath::RInterpConstantTo(TargetRotation, Target, DeltaTime, TargetInterpSpeed);
	SetActorRotation(
		FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, ActorInterpSpeed));
}

/* 设置角色平滑过度旋转(限制版)*/
void AyyBaseCharacter::LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime)
{
	FRotator Delta = AimingRotation - GetActorRotation();
	Delta.Normalize();
	const float RangeVal = Delta.Yaw;
	if (RangeVal < AimYawMin || RangeVal > AimYawMax)
	{
		const float ControlRotYaw = AimingRotation.Yaw;
		const float TargetYaw = ControlRotYaw + (RangeVal > 0.0f ? AimYawMin : AimYawMax);
		SmoothCharacterRotation({0.0f, TargetYaw, 0.0f}, 0.0f, InterpSpeed, DeltaTime);
	}
}

/* 设置平滑过度空中旋转值*/
void AyyBaseCharacter::UpdateInAirRotation(float DeltaTime)
{
	if (RotationMode == EyyRotationMode::VelocityDirection || RotationMode == EyyRotationMode::LookingDirection)
	{
		// Velocity / Looking Direction Rotation
		SmoothCharacterRotation({0.0f, InAirRotation.Yaw, 0.0f}, 0.0f, 5.0f, DeltaTime);
	}
	else if (RotationMode == EyyRotationMode::Aiming)
	{
		// Aiming Rotation
		SmoothCharacterRotation({0.0f, AimingRotation.Yaw, 0.0f}, 0.0f, 15.0f, DeltaTime);
		InAirRotation = GetActorRotation();
	}
}

/* 设置布娃娃过度旋转值*/
void AyyBaseCharacter::RagdollUpdate(float DeltaTime)
{
	GetMesh()->bOnlyAllowAutonomousTickPose = false;
	const FVector NewRagdollVel = GetMesh()->GetPhysicsLinearVelocity(NAME_root);
	LastRagdollVelocity = (NewRagdollVel != FVector::ZeroVector || IsLocallyControlled())
							  ? NewRagdollVel
							  : LastRagdollVelocity / 2;
	const float SpringValue = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 1000.0f}, {0.0f, 25000.0f},
	                                                            LastRagdollVelocity.Size());
	GetMesh()->SetAllMotorsAngularDriveParams(SpringValue, 0.0f, 0.0f, false);
	const bool bEnableGrav = LastRagdollVelocity.Z > -4000.0f;
	GetMesh()->SetEnableGravity(bEnableGrav);
	SetActorLocationDuringRagdoll(DeltaTime);
}

void AyyBaseCharacter::SetActorLocationDuringRagdoll(float DeltaTime)
{
	if (IsLocallyControlled())
	{
		TargetRagdollLocation = GetMesh()->GetSocketLocation(NAME_Pelvis);
	}
	const FRotator PelvisRot = GetMesh()->GetSocketRotation(NAME_Pelvis);
	if (bReversedPelvis) {
		bRagdollFaceUp = PelvisRot.Roll > 0.0f;
	} else
	{
		bRagdollFaceUp = PelvisRot.Roll < 0.0f;
	}
	const FRotator TargetRagdollRotation(0.0f, bRagdollFaceUp ? PelvisRot.Yaw - 180.0f : PelvisRot.Yaw, 0.0f);
	const FVector TraceVect(TargetRagdollLocation.X, TargetRagdollLocation.Y,
						TargetRagdollLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	UWorld* World = GetWorld();
	check(World);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(HitResult, TargetRagdollLocation, TraceVect,
													  ECC_Visibility, Params);
	bRagdollOnGround = HitResult.IsValidBlockingHit();
	FVector NewRagdollLoc = TargetRagdollLocation;
	if (bRagdollOnGround)
	{
		const float ImpactDistZ = FMath::Abs(HitResult.ImpactPoint.Z - HitResult.TraceStart.Z);
		NewRagdollLoc.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - ImpactDistZ + 2.0f;
	}
	SetActorLocationAndTargetRotation(bRagdollOnGround ? NewRagdollLoc : TargetRagdollLocation, TargetRagdollRotation);
}

void AyyBaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (GetCharacterMovement()->MovementMode == MOVE_Walking ||
		GetCharacterMovement()->MovementMode == MOVE_NavWalking)
	{
		SetMovementState(EyyMovementState::Grounded);
	}
	else if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		SetMovementState(EyyMovementState::InAir);
	}
}

// Called every frame
void AyyBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetEssentialValues(DeltaTime);
	
	if (MovementState == EyyMovementState::Grounded)
	{
		UpdateCharacterMovement();
		UpdateGroundedRotation(DeltaTime);
	}
	else if (MovementState == EyyMovementState::InAir)
	{
		UpdateInAirRotation(DeltaTime);
	}
	else if (MovementState == EyyMovementState::Ragdoll)
	{
		RagdollUpdate(DeltaTime);
	}
		
	// Cache values
	PreviousVelocity = GetVelocity();
	PreviousAimYaw = AimingRotation.Yaw;
}

void AyyBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// DOREPLIFETIME(AyyBaseCharacter, TargetRagdollLocation);
	// DOREPLIFETIME_CONDITION(AyyBaseCharacter, ReplicatedCurrentAcceleration, COND_SkipOwner);
	// DOREPLIFETIME_CONDITION(AyyBaseCharacter, ReplicatedControlRotation, COND_SkipOwner);
	//
	// DOREPLIFETIME(AyyBaseCharacter, DesiredGait);
	// DOREPLIFETIME_CONDITION(AyyBaseCharacter, DesiredStance, COND_SkipOwner);
	// DOREPLIFETIME_CONDITION(AyyBaseCharacter, DesiredRotationMode, COND_SkipOwner);
	//
	// DOREPLIFETIME_CONDITION(AyyBaseCharacter, RotationMode, COND_SkipOwner);
	// DOREPLIFETIME_CONDITION(AyyBaseCharacter, OverlayState, COND_SkipOwner);
	// DOREPLIFETIME_CONDITION(AyyBaseCharacter, ViewMode, COND_SkipOwner);
	// DOREPLIFETIME_CONDITION(AyyBaseCharacter, VisibleMesh, COND_SkipOwner);
}

// Called to bind functionality to input
void AyyBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/* 更新人物朝向状态标志 */
void AyyBaseCharacter::SetRotationMode(EyyRotationMode NewRotationMode, bool bForce)
{
	if (bForce || RotationMode != NewRotationMode)
	{
		const EyyRotationMode Prev = RotationMode;
		RotationMode = NewRotationMode;
		OnRotationModeChanged();
	}
}

/* 根据状态标志, 返回允许的步伐模式, Sprint需要条件才能触发 */
EyyGait AyyBaseCharacter::GetAllowedGait() const
{
	if (Stance == EyyStance::Standing)
	{
		if (RotationMode != EyyRotationMode::Aiming)
		{
			if (DesiredGait == EyyGait::Sprinting)
			{
				return CanSprint() ? EyyGait::Sprinting : EyyGait::Running;
			}
			return DesiredGait;
		}
	}
	if (DesiredGait == EyyGait::Sprinting)
	{
		return EyyGait::Running;
	}
	return DesiredGait;
}

/* 是否可以冲刺 */
bool AyyBaseCharacter::CanSprint() const
{
	if (!bHasMovementInput || RotationMode == EyyRotationMode::Aiming)
	{
		return false;
	}
	const bool bValidInputAmount = MovementInputAmount > 0.9f;
	if (RotationMode == EyyRotationMode::VelocityDirection)
	{
		return bValidInputAmount;
	}
	if (RotationMode == EyyRotationMode::LookingDirection)
	{
		const FRotator AccRot = ReplicatedCurrentAcceleration.ToOrientationRotator();
		FRotator Delta = AccRot - AimingRotation;
		Delta.Normalize();
		return bValidInputAmount && FMath::Abs(Delta.Yaw) < 50.0f;
	}
	return false;
}

FVector AyyBaseCharacter::GetFirstPersonCameraTarget()
{
	return GetMesh()->GetSocketLocation(NAME_FP_Camera);
}

void AyyBaseCharacter::GetCameraParameters(float& TPFOVOut, float& FPFOVOut, bool& bRightShoulderOut) const
{
	TPFOVOut = ThirdPersonFOV;
	FPFOVOut = FirstPersonFOV;
	bRightShoulderOut = bRightShoulder;
}

ECollisionChannel AyyBaseCharacter::GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius)
{
	TraceOrigin = GetActorLocation();
	TraceRadius = 10.0f;
	return ECC_Visibility;
}

/* 更新人物以及Camera的Gait状态*/
void AyyBaseCharacter::SetGait(EyyGait NewGait, bool bForce)
{
	if (bForce || Gait != NewGait)
	{
		const EyyGait prev = Gait;
		Gait = NewGait;
		OnGaitChanged(prev);
	}
}

/*更新人物以及Camera的Stance状态标志*/
void AyyBaseCharacter::SetStance(EyyStance NewStance, bool bForce)
{
	if (bForce || Stance != NewStance)
	{
		const EyyStance prev = Stance;
		Stance = NewStance;
		OnStanceChanged(prev);
	}
}

/* 更新视角模式的状态标志 */
void AyyBaseCharacter::SetViewMode(EyyViewMode NewViewMode, bool bForce)
{
	if (bForce || ViewMode != NewViewMode)
	{
		ViewMode = NewViewMode;
		OnViewModeChanged();
	}
}

/* 更新人物模型的状态标志 */
void AyyBaseCharacter::SetOverlayState(EyyOverlayState NewState, bool bForce)
{
	if (bForce || OverlayState != NewState)
	{
		const EyyOverlayState Prev = OverlayState;
		OverlayState = NewState;
		OnOverlayStateChanged(Prev);
	}
}

/*
 *设置 持久运动状态: 地上\坠落\攀爬\失去意识的状态标志
 * 更新CameraBehavior的MovementState状态标志
 */
void AyyBaseCharacter::SetMovementState(EyyMovementState NewState, bool bForce)
{
	if (bForce || MovementState != NewState)
	{
		PrevMovementState = MovementState;
		MovementState = NewState;
		OnMovementStateChanged(PrevMovementState);
	}
}

/*
 * 设置运动动作的状态标志
 * 更新CameraBehavior的MovementAction状态标志
 */
void AyyBaseCharacter::SetMovementAction(EyyMovementAction NewAction, bool bForce)
{
	if (bForce || MovementAction != NewAction)
	{
		const EyyMovementAction Prev = MovementAction;
		MovementAction = NewAction;
		OnMovementActionChanged(Prev);
	}
}

FyyMovementSettings AyyBaseCharacter::GetTargetMovementSettings() const
{
	if (RotationMode == EyyRotationMode::VelocityDirection)
	{
		if (Stance == EyyStance::Standing)
		{
			return MovementData.VelocityDirection.Standing;
		}
		if (Stance == EyyStance::Crouching)
		{
			return MovementData.VelocityDirection.Crouching;
		}
	}
	else if (RotationMode == EyyRotationMode::LookingDirection)
	{
		if (Stance == EyyStance::Standing)
		{
			return MovementData.LookingDirection.Standing;
		}
		if (Stance == EyyStance::Crouching)
		{
			return MovementData.LookingDirection.Crouching;
		}
	}
	else if (RotationMode == EyyRotationMode::Aiming)
	{
		if (Stance == EyyStance::Standing)
		{
			return MovementData.Aiming.Standing;
		}
		if (Stance == EyyStance::Crouching)
		{
			return MovementData.Aiming.Crouching;
		}
	}
	return MovementData.VelocityDirection.Standing;
}

/* 根据速度获取实际步伐状态*/
EyyGait AyyBaseCharacter::GetActualGait(EyyGait AllowedGait) const
{
	const float LocWalkSpeed = MyCharacterMovementComponent->CurrentMovementSettings.WalkSpeed;
	const float LocRunSpeed = MyCharacterMovementComponent->CurrentMovementSettings.RunSpeed;
	if (Speed > LocRunSpeed + 10.0f)
	{
		if (AllowedGait == EyyGait::Sprinting)
		{
			return EyyGait::Sprinting;
		}
		return EyyGait::Running;
	}
	if (Speed >= LocWalkSpeed + 10.0f)
	{
		return EyyGait::Running;
	}
	return EyyGait::Walking;
}

/*获取CurveName当前值, 值由系统设置*/
float AyyBaseCharacter::GetAnimCurveValue(FName CurveName) const
{
	if (GetMesh()->GetAnimInstance())
	{
		return GetMesh()->GetAnimInstance()->GetCurveValue(CurveName);
	}
	return 0.0f;
}

void AyyBaseCharacter::SetActorLocationAndTargetRotation(FVector NewLocation, FRotator NewRotation)
{
	SetActorLocationAndRotation(NewLocation, NewRotation);
	TargetRotation = NewRotation;
}

FTransform AyyBaseCharacter::GetThirdPersonPivotTarget()
{
	return GetActorTransform();
}

// void AyyBaseCharacter::ReplicatedRagdollStart()
// {
// 	if (HasAuthority())
// 	{
// 		Multicast_RagdollStart();
// 	}
// }
// 	void AyyBaseCharacter::Multicast_RagdollStart()
// 	{
// 		RagdollStart();
// 	}