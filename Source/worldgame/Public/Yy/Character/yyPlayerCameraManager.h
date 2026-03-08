// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "yyPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class WORLDGAME_API AyyPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AyyPlayerCameraManager();
	
	UFUNCTION(BlueprintCallable, Category = "ALS|Camera")
	void OnPossess(AyyBaseCharacter* NewCharacter);
	
	
	UFUNCTION(BlueprintCallable, Category = "ALS|Camera")
	float GetCameraBehaviorParam(FName CurveName) const;
	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<AyyBaseCharacter> ControlledCharacter = nullptr;
	/* PCM的骨骼 但是隐藏, 用于引用动画蓝图*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<USkeletalMeshComponent> CameraBehavior = nullptr;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ALS|Camera")
	FTransform SmoothedPivotTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ALS|Camera")
	FRotator TargetCameraRotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera")
	FRotator DebugViewRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ALS|Camera")
	FVector PivotLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ALS|Camera")
	FVector TargetCameraLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera")
	FVector DebugViewOffset;
	
	UFUNCTION(BlueprintCallable, Category = "ALS|Camera")
	bool CustomCameraBehavior(float DeltaTime, FVector& Location, FRotator& Rotation, float& FOV);
	UFUNCTION(BlueprintCallable, Category = "ALS|Camera")
	static FVector CalculateAxisIndependentLag(
		FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeeds, float DeltaTime);

};
