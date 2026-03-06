#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "yyPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class WORLDGAME_API AyyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AyyPlayerController() {};
	virtual void OnPossess(APawn* aPawn) override;
	/* 在OnPossess后调用 */
	virtual void SetupInputComponent() override;
	void BindAction(UInputMappingContext* context);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext = nullptr;
	
private:
	void SetupInputs() const;
	class AyyBaseCharacter* PossessedCharacter = nullptr;
};
