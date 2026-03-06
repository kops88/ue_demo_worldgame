#include "Yy/Character/yyBaseCharacter.h"
#include "Yy/Character/yyPlayerCameraBehavior.h"
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
	
	
	
	
	
}

void AyyBaseCharacter::SetMovementModel()
{
	const FString ContextString = GetFullName();
	FyyMovementStateSettings* OutRow = MovementModel.DataTable->FindRow<FyyMovementStateSettings>(MovementModel.RowName, ContextString)
	check(OutRow);
	MovementData = *OutRow;
}

void AyyBaseCharacter::ForceUpdateCharacterState()
{
	SetGait(DesiredGait, true);
	SetStance(DesiredStance, true);
	SetRotationMode(DesiredRotationMode, true);
	SetViewMode(ViewMode, true);
	SetOverlayState(OverlayState, true);
	
}

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

void AyyBaseCharacter::OnMovementStateChanged(EyyMovementState PreviousState)
{
	if (MovementState == EyyMovementState::InAir)
	{
	//7758;		
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

/* 更新人物朝向状态 */
void AyyBaseCharacter::SetRotationMode(EyyRotationMode NewRotationMode, bool bForce)
{
	if (bForce || RotationMode != NewRotationMode)
	{
		const EyyRotationMode Prev = RotationMode;
		RotationMode = NewRotationMode;
		OnRotationModeChanged();
	}
}

void AyyBaseCharacter::SetGait(EyyGait NewGait, bool bForce)
{
	if (bForce || Gait != NewGait)
	{
		const EyyGait prev = Gait;
		Gait = NewGait;
		OnGaitChanged(prev);
	}
}

void AyyBaseCharacter::SetStance(EyyStance NewStance, bool bForce)
{
	if (bForce || Stance != NewStance)
	{
		const EyyStance prev = Stance;
		Stance = NewStance;
		OnStanceChanged(prev);
	}
}

/* 更新视角模式的标志 */
void AyyBaseCharacter::SetViewMode(EyyViewMode NewViewMode, bool bForce)
{
	if (bForce || ViewMode != NewViewMode)
	{
		ViewMode = NewViewMode;
		OnViewModeChanged();
	}
}

/* 更新人物模型的标志 */
void AyyBaseCharacter::SetOverlayState(EyyOverlayState NewState, bool bForce)
{
	if (bForce || OverlayState != NewState)
	{
		const EyyOverlayState Prev = OverlayState;
		OverlayState = NewState;
		OnOverlayStateChanged(Prev);
	}
}

void AyyBaseCharacter::SetMovementState(EyyMovementState NewState, bool bForce)
{
	if (bForce || MovementState != NewState)
	{
		PrevMovementState = MovementState;
		MovementState = NewState;
		OnMovementStateChanged(PrevMovementState);
		
	}
	
}

