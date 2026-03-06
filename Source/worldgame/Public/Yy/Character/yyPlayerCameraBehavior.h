// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "yyPlayerCameraBehavior.generated.h"

/**
 * 
 */
UCLASS()
class WORLDGAME_API UyyPlayerCameraBehavior : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	void SetRotationMode(EyyRotationMode RotationMode);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	EyyStance Stance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	EyyGait Gait;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	EyyViewMode ViewMode;
	
	/* 是否 角色不跟随镜头方向 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	bool bVelocityDirection = false;
	/* 是否 角色跟随镜头方向*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	bool bLookingDirection = false;
	/* 是否 瞄准模式*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Read Only Data|Character Information")
	bool bAiming = false;

	
};
