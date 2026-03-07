#include "Yy/Character/yyBaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Yy/Character/yyPlayerCameraBehavior.h"


const FName NAME_Pelvis(TEXT("Pelvis"));



// Sets default values
AyyBaseCharacter::AyyBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

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

void AyyBaseCharacter::SetMovementModel()
{
	const FString ContextString = GetFullName();
	FyyMovementStateSettings* OutRow = MovementModel.DataTable->FindRow<FyyMovementStateSettings>(MovementModel.RowName, ContextString)
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
		else if (MovementAction == EyyMovementAction::Rolling)
		{
			ReplicatedRagdollStart();
		}
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

// Called every frame
void AyyBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void AyyBaseCharacter::ReplicatedRagdollStart()
{
	if (HasAuthority())
	{
		Multicast_RagdollStart();
	}
}

void AyyBaseCharacter::Multicast_RagdollStart_Implementation()
{
	RagdollStart();
}

