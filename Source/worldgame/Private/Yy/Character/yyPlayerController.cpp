#include "Yy/Character/yyPlayerController.h"
#include "Yy/Character/yyBaseCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"

/*初始化*/
void AyyPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	PossessedCharacter = Cast<AyyBaseCharacter>(aPawn);
	SetupInputs();
	if (!IsValid(PossessedCharacter)) return;
}



void AyyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	APlayerController::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->ClearActionEventBindings();
		EnhancedInputComponent->ClearActionValueBindings();
		EnhancedInputComponent->ClearDebugKeyBindings();

		BindAction(DefaultInputMappingContext);
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
			EnhancedInputSubsystem->AddMappingContext(DefaultInputMappingContext, 0, options);
		}
	}
}
