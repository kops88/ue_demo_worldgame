#include "Yy/Character/yyPlayerController.h"
#include "Yy/Character/yyBaseCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Yy/Character/yyPlayerCameraManager.h"


/*初始化*/
void AyyPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	PossessedCharacter = Cast<AyyBaseCharacter>(aPawn);
	if (!IsRunningDedicatedServer())
	{
		// 非服务器需要执行
		SetupCamera();
	}
	SetupInputs();
	if (!IsValid(PossessedCharacter)) return;
}



void AyyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->ClearActionEventBindings();
		EnhancedInputComponent->ClearActionValueBindings();
		EnhancedInputComponent->ClearDebugKeyBindings();

		BindAction(DefaultInputMappingContext1);
	}

}

void AyyPlayerController::BindAction(UInputMappingContext* context)
{
	if (context)
	{
		const TArray<FEnhancedActionKeyMapping>& Mappings = context->GetMappings();
		/* 项目启用了增强输入, 所以可以转换 */
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		if (EnhancedInputComponent)
		{
			TSet<const UInputAction*> Actions;
			for (const FEnhancedActionKeyMapping& KeyMapping : Mappings)
			{
				Actions.Add(KeyMapping.Action);
			}
			for (const UInputAction* action : Actions)
			{
				EnhancedInputComponent->BindAction(action, ETriggerEvent::Triggered, Cast<UObject>(this), action->GetFName());
			}
		}
	}
}

void AyyPlayerController::ForwardMovementAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->ForwardMovementAction(Value.GetMagnitude());
	}
}

void AyyPlayerController::RightMovementAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->RightMovementAction(Value.GetMagnitude());
	}
}

void AyyPlayerController::SetupCamera()
{
	AyyPlayerCameraManager* CastedMgr = Cast<AyyPlayerCameraManager>(PlayerCameraManager);
	if (PossessedCharacter && CastedMgr)
	{
		CastedMgr->OnPossess(PossessedCharacter);
	}
}

void AyyPlayerController::SetupInputs() const
{
	if (PossessedCharacter)
	{
		/* 管理映射上下文 */
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (EnhancedInputSubsystem)
		{
			FModifyContextOptions options;
			options.bForceImmediately = true;
			EnhancedInputSubsystem->AddMappingContext(DefaultInputMappingContext1, 1, options);
		}
	}
}
