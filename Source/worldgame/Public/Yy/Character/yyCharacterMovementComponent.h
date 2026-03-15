// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Yy/enum/yyCharacterStructLibrary.h"
#include "yyCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class WORLDGAME_API UyyCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	public:
	
	virtual void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity) override;
	
	
	// Movement Settings Variables
	UPROPERTY()
	uint8 bRequestMovementSettingsChange = 1;
	
	float GetMappedSpeed() const;
	UFUNCTION(BlueprintCallable, Category = "")
	void SetMovementSettings(FyyMovementSettings NewMovementSettings);
	UFUNCTION(BlueprintCallable, Category = "Movement Settings")
	void SetAllowedGait(EyyGait NewAllowedGait);
	/* 运行时数据 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement System")
	FyyMovementSettings CurrentMovementSettings;
	UPROPERTY()
	EyyGait AllowedGait = EyyGait::Walking;
	
};
