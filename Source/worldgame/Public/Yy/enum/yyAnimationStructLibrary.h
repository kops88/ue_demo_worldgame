#pragma once
#include "yyCharacterStructLibrary.h"

/* 用于动画蓝图中的空中信息*/
USTRUCT(BlueprintType)
struct FyyAnimGraphInAir
{
	GENERATED_BODY()
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - In Air")
	bool bJumped = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - In Air")
	float JumpPlayRate = 1.2f;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - In Air")
	float FallSpeed = 0.0f;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - In Air")
	float LandPrediction = 1.0f;
	
};

USTRUCT(BlueprintType)
struct FyyAnimCharacterInformation
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	FRotator AimingRotation = FRotator::ZeroRotator;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	FRotator CharacterActorRotation = FRotator::ZeroRotator;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	FVector Velocity = FVector::ZeroVector;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	FVector RelativeVelocityDirection = FVector::ZeroVector;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	FVector Acceleration = FVector::ZeroVector;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	FVector MovementInput = FVector::ZeroVector;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	bool bIsMoving = false;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	bool bHasMovementInput = false;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	float Speed = 0.0f;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	float MovementInputAmount = 0.0f;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	float AimYawRate = 0.0f;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	float ZoomAmount = 0.0f;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	EyyMovementState PrevMovementState = EyyMovementState::None;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Character Information")
	EyyViewMode ViewMode = EyyViewMode::ThirdPerson;
};
































