#include "Yy/AnimInstance/yyCharacterAnimInstance.h"
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

/* 设置空中状态true, 定时0.1秒后设为false */
void UyyCharacterAnimInstance::OnJumped()
{
	InAir.bJumped = true;
	InAir.JumpPlayRate = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 600.0f}, {1.2f, 1.5f}, CharacterInformation.Speed);
	GetWorld()->GetTimerManager().SetTimer(OnJumpedTimer, this,
									  &UyyCharacterAnimInstance::OnJumpedDelay, 0.1f, false);
}

void UyyCharacterAnimInstance::OnJumpedDelay()
{
	InAir.bJumped = false;
}
