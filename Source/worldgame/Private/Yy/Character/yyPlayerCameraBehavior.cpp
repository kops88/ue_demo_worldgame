// Fill out your copyright notice in the Description page of Project Settings.

#include "Yy/Character/yyPlayerCameraBehavior.h"
#include "Yy/enum/yyCharacterStructLibrary.h"

/* 更新角色朝向模式的标志位 */
void UyyPlayerCameraBehavior::SetRotationMode(EyyRotationMode RotationMode)
{
	bVelocityDirection = RotationMode == EyyRotationMode::VelocityDirection;
	bLookingDirection = RotationMode == EyyRotationMode::LookingDirection;
	bAiming = RotationMode == EyyRotationMode::Aiming;
	
}
