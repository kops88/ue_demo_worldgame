#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Yy/enum/yyAnimationStructLibrary.h"
#include "Yy/enum/yyCharacterStructLibrary.h"
#include "yyCharacterAnimInstance.generated.h"

UCLASS()
class WORLDGAME_API UyyCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	/*初始化动画实例*/
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override {};
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
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
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Layer Blending", Meta = (
		ShowOnlyInnerProperties))
	FyyAnimGraphLayerBlending LayerBlendingValues;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	FyyMovementState MovementState = EyyMovementState::None;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	FyyMovementAction MovementAction = EyyMovementAction::None;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	FyyStance Stance = EyyStance::Standing;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	FyyRotationMode RotationMode = EyyRotationMode::VelocityDirection;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	FyyGait Gait = EyyGait::Walking;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	FyyOverlayState OverlayState = EyyOverlayState::Default;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	FyyGroundedEntryState GroundedEntryState = EyyGroundedEntryState::None;
	
	
	
	
private:
	void OnJumpedDelay();
	
	void UpdateAimingValues(float DeltaSeconds);
	
	
	
	
	
	
	FTimerHandle OnJumpedTimer;
	
	
	
	
	
};
