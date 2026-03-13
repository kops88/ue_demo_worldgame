#include "Yy/AnimInstance/yyCharacterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Yy/Character/yyBaseCharacter.h"
// AyyBaseCharacter




void UyyCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Character = Cast<AyyBaseCharacter>(TryGetPawnOwner());
	if (Character)
	{
		Character->OnJumpedDelegate.AddUniqueDynamic(this, &UyyCharacterAnimInstance::OnJumped);
	}
}

void UyyCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!Character || DeltaSeconds == 0.0f)
	{
		return;
	}
	CharacterInformation.MovementInputAmount = Character->GetMovementInputAmount();
	CharacterInformation.bHasMovementInput = Character->HasMovementInput();
	CharacterInformation.bIsMoving = Character->IsMoving();
	CharacterInformation.Acceleration = Character->GetAcceleration();
	CharacterInformation.AimYawRate = Character->GetAimYawRate();
	CharacterInformation.Speed = Character->GetSpeed();
	CharacterInformation.Velocity = Character->GetCharacterMovement()->Velocity;
	CharacterInformation.MovementInput = Character->GetMovementInput();
	CharacterInformation.AimingRotation = Character->GetAimingRotation();
	CharacterInformation.CharacterActorRotation = Character->GetActorRotation();
	CharacterInformation.ViewMode = Character->GetViewMode();
	CharacterInformation.PrevMovementState = Character->GetPrevMovementState();
	LayerBlendingValues.OverlayOverrideState = Character->GetOverlayOverrideState();
	MovementState = Character->GetMovementState();
	MovementAction = Character->GetMovementAction();
	Stance = Character->GetStance();
	RotationMode = Character->GetRotationMode();
	Gait = Character->GetGait();
	OverlayState = Character->GetOverlayState();
	GroundedEntryState = Character->GetGroundedEntryState();
	
	UpdateAimingValues(DeltaSeconds);
	
	
	
	
	
	
	
	
	
}




/* 设置空中状态true, 定时0.1秒后设为false */
void UyyCharacterAnimInstance::OnJumped()
{
	InAir.bJumped = true;
	InAir.JumpPlayRate = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 600.0f}, {1.2f, 1.5f}, CharacterInformation.Speed);
	GetWorld()->GetTimerManager().SetTimer(OnJumpedTimer, this,
									  &UyyCharacterAnimInstance::OnJumpedDelay, 0.1f, false);
}



/* 设置空中状态false */
void UyyCharacterAnimInstance::OnJumpedDelay()
{
	InAir.bJumped = false;
}

void UyyCharacterAnimInstance::UpdateAimingValues(float DeltaSeconds)
{
	
	
	
}
