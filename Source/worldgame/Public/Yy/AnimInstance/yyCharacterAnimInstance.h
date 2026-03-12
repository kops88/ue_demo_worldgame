#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Yy/enum/yyAnimationStructLibrary.h"
#include "yyCharacterAnimInstance.generated.h"

UCLASS()
class WORLDGAME_API UyyCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	/*初始化动画实例*/
	virtual void NativeInitializeAnimation() override;
	
	UFUNCTION(BlueprintCallable, Category = "Event")
	void OnJumped();
	
	
	
	
	
public:
	/** References */
	UPROPERTY(BlueprintReadOnly, Category = "Read Only Data|Character Information")
	TObjectPtr<AyyBaseCharacter> Character = nullptr;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - In Air", Meta = (
	ShowOnlyInnerProperties))
	FyyAnimGraphInAir InAir;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information", Meta = (
	ShowOnlyInnerProperties))
	FyyAnimCharacterInformation CharacterInformation;
	
	
private:
	void OnJumpedDelay();
	
	
	
	
	
	
	
	FTimerHandle OnJumpedTimer;
	
	
	
	
	
};
