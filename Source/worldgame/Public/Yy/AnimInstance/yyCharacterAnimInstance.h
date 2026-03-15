#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Yy/enum/yyAnimationStructLibrary.h"
#include "Yy/enum/yyCharacterStructLibrary.h"
#include "Yy/Character/yyBaseCharacter.h"
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
	/* 
	 * 用于状态机move的转换条件
	 * BlendPose用的枚举
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	FyyGait Gait = EyyGait::Walking;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	FyyOverlayState OverlayState = EyyOverlayState::Default;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	FyyGroundedEntryState GroundedEntryState = EyyGroundedEntryState::None;
	
	/** Anim Graph - Aiming Values */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Aiming Values", Meta = (
		ShowOnlyInnerProperties))
	FyyAnimGraphAimingValues AimingValues;
	
	
	/** Configuration */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Main Configuration", Meta = (
		ShowOnlyInnerProperties))
	FyyAnimConfiguration Config;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Aiming Values")
	FVector2D SmoothedAimingAngle = FVector2D::ZeroVector;
	
	/** IK Bone Names */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Anim Graph - Foot IK")
	FName IkFootL_BoneName = FName(TEXT("ik_foot_l"));
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Anim Graph - Foot IK")
	FName IkFootR_BoneName = FName(TEXT("ik_foot_r"));
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Foot IK", Meta = (
		ShowOnlyInnerProperties))
	FyyAnimGraphFootIK FootIKValues;
	
	/* 
	 * 状态机中Ground N 的转换条件
	 * WalkRun的混合空间的 stride-standingPlayRate, walk/run - walkRunBlend, playrate
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded", Meta = (
		ShowOnlyInnerProperties))
	FyyAnimGraphGrounded Grounded;
	/** Turn In Place */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Turn In Place", Meta = (
		ShowOnlyInnerProperties))
	FyyAnimTurnInPlace TurnInPlaceValues;
	
	/* 用于状态机中RunStart的动画序列BlendMulti混合的FBRL权重*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	FyyVelocityBlend VelocityBlend;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveFloat> DiagonalScaleAmountCurve = nullptr;
	/* 
	 * x: 用于混合 sprint_f 和 sprint_f_impulse
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	FVector RelativeAccelerationAmount = FVector::ZeroVector;
	/*
	 * 用于混合空间 yy_n_lean 的轴输入
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	FyyLeanAmount LeanAmount;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveFloat> StrideBlend_N_Walk = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveFloat> StrideBlend_N_Run = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveFloat> StrideBlend_C_Walk = nullptr;
	
	/* 控制N cycleBlending 中 N DirectionState 中的转换条件 (常驻效果) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Grounded")
	FyyMovementDirection MovementDirection = EyyMovementDirection::Forward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveVector> YawOffset_FB = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Blend Curves")
	TObjectPtr<UCurveVector> YawOffset_LR = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Rotate In Place", Meta = (
			ShowOnlyInnerProperties))
	FyyAnimRotateInPlace RotateInPlace;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Dynamic Transition")
	TObjectPtr<UAnimSequenceBase> TransitionAnim_R = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Dynamic Transition")
	TObjectPtr<UAnimSequenceBase> TransitionAnim_L = nullptr;
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation")
	void PlayDynamicTransition(float ReTriggerDelay, FyyDynamicMontageParams Parameters);
	
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation")
	void PlayTransition(const FyyDynamicMontageParams& Parameters);
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read Only Data|Anim Graph - Ragdoll")
	float FlailRate = 0.0f;
	
	protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Grounded")
	bool ShouldMoveCheck() const;
	UFUNCTION(BlueprintCallable, Category = "ALS|Grounded")
	bool CanRotateInPlace() const;
	void RotateInPlaceCheck();
	UFUNCTION(BlueprintCallable, Category = "ALS|Grounded")
	bool CanTurnInPlace() const;
	UFUNCTION(BlueprintCallable, Category = "ALS|Grounded")
	bool CanDynamicTransition() const;
	
	
	
private:
	void UpdateRagdollValues();
	void UpdateInAirValues(float DeltaSeconds);
	void PlayDynamicTransitionDelay();
	void DynamicTransitionCheck();
	
	void OnJumpedDelay();
	
	void UpdateAimingValues(float DeltaSeconds);
	
	void UpdateLayerValues();
	
	void UpdateFootIK(float DeltaSeconds);
	
	void UpdateMovementValues(float DeltaSeconds);
	
	float GetAnimCurveClamped(const FName& Name, float Bias, float ClampMin, float ClampMax) const;
	
	void UpdateRotationValues();
	
	/** Grounded */
	void TurnInPlaceCheck(float DeltaSeconds);
	void TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool OverrideCurrent);
	
	FTimerHandle PlayDynamicTransitionTimer;
	bool bCanPlayDynamicTransition = true;
	FyyVelocityBlend CalculateVelocityBlend() const;
	float CalculateDiagonalScaleAmount() const;
	FVector CalculateRelativeAccelerationAmount() const;
	float CalculateWalkRunBlend() const;
	float CalculateStrideBlend() const;
	float CalculateStandingPlayRate() const;
	float CalculateCrouchingPlayRate() const;
	EyyMovementDirection CalculateMovementDirection() const;
	
	/** Foot IK */
	void SetFootLocking(float DeltaSeconds, FName EnableFootIKCurve, FName FootLockCurve, FName IKFootBone,
						  float& CurFootLockAlpha, bool& UseFootLockCurve,
						  FVector& CurFootLockLoc, FRotator& CurFootLockRot);
	void SetFootLockOffsets(float DeltaSeconds, FVector& LocalLoc, FRotator& LocalRot);
	void SetPelvisIKOffset(float DeltaSeconds, FVector FootOffsetLTarget, FVector FootOffsetRTarget);
	void ResetIKOffsets(float DeltaSeconds);
	void SetFootOffsets(float DeltaSeconds, FName EnableFootIKCurve, FName IKFootBone, FName RootBone,
						  FVector& CurLocationTarget, FVector& CurLocationOffset, FRotator& CurRotationOffset);

	
	FTimerHandle OnJumpedTimer;
	
	
	// math
	public:
	
	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static bool AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool IncreaseBuffer);
	
	
	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static EyyMovementDirection CalculateQuadrant(EyyMovementDirection Current, float FRThreshold, float FLThreshold,
												   float BRThreshold,
												   float BLThreshold, float Buffer, float Angle);
	
};
