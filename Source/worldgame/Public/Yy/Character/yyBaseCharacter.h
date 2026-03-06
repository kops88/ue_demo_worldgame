#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Yy/enum/yyCharacterStructLibrary.h"
#include "yyBaseCharacter.generated.h"

USTRUCT(BlueprintType)
struct FyyMovementSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	float WalkSpeed = 0.0f;
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	float RunSpeed = 0.0f;
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	float SprintSpeed = 0.0f;
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	TObjectPtr<UCurveVector> MovementCurve = nullptr;
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	TObjectPtr<UCurveFloat> RotationRateCurve = nullptr;
};

USTRUCT(BlueprintType)
struct FALSMovementStanceSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	FyyMovementSettings Standing;
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	FyyMovementSettings Crouching;
};

USTRUCT(BlueprintType)
struct FyyMovementStateSettings : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	FALSMovementStanceSettings VelocityDirection;
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	FALSMovementStanceSettings LookingDirection;
	UPROPERTY(EditAnywhere, Category = "MovementSettings")
	FALSMovementStanceSettings Aiming;
};

UCLASS()
class WORLDGAME_API AyyBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AyyBaseCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SetRotationMode(EyyRotationMode NewRotationMode, bool bForce = false);

	
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetGait(EyyGait NewGait, bool bForce = false);
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetStance(EyyStance NewStance, bool bForce = false);
	
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetViewMode(EyyViewMode NewViewMode, bool bForce = false);
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetOverlayState(EyyOverlayState NewState, bool bForce = false);
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetMovementState(EyyMovementState NewState, bool bForce = false);
	
protected:
	void SetMovementModel();
	void ForceUpdateCharacterState();
	
	virtual void OnGaitChanged(EyyGait PreviousGait);
	virtual void OnStanceChanged(EyyStance PreviousStance);
	virtual void OnRotationModeChanged();
	virtual void OnViewModeChanged();
	virtual void OnOverlayStateChanged(EyyOverlayState PreviousState);
	virtual void OnMovementStateChanged(EyyMovementState PreviousState);
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement System")
	FDataTableRowHandle MovementModel;
	UPROPERTY(BlueprintReadOnly, Category = "Movement System")
	FyyMovementStateSettings MovementData;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Input")
	EyyRotationMode DesiredRotationMode = EyyRotationMode::LookingDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EyyGait DesiredGait = EyyGait::Running;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EyyStance DesiredStance = EyyStance::Standing;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Values")
	EyyOverlayState OverlayState = EyyOverlayState::Default;

	
	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EyyGait Gait = EyyGait::Walking;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Values")
	EyyStance Stance = EyyStance::Standing;
	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EyyRotationMode RotationMode = EyyRotationMode::LookingDirection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Values")
	EyyViewMode ViewMode = EyyViewMode::ThirdPerson;
	/* 持久性运动状态 */
	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EyyMovementState MovementState = EyyMovementState::None;
	/* 前一次 持久性运动状态 */
	UPROPERTY(BlueprintReadOnly, Category = "State Values")
	EyyMovementState PrevMovementState = EyyMovementState::None;
	

	/* camera */
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UyyPlayerCameraBehavior> CameraBehavior;
	
};














