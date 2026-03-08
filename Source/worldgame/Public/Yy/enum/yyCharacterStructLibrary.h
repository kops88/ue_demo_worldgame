#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Materials/MaterialInterface.h"
#include "yyCharacterStructLibrary.generated.h"

/* 步伐模式*/
UENUM(BlueprintType, meta = (ScriptName = "yy_Gait"))
enum class EyyGait : uint8
{
	Walking,
	Running,
	Sprinting
};

/* 站or蹲*/
UENUM(BlueprintType, meta = (ScriptName = "yy_Stance"))
enum class EyyStance : uint8
{
	/* 站 */
	Standing,
	/* 蹲 */
	Crouching
};

/*角色朝向模式*/
UENUM(BlueprintType, meta = (ScriptName="RotationMode"))
enum class EyyRotationMode : uint8
{
	/* 角色不跟随镜头方向 */
	VelocityDirection,
	/* 角色跟随镜头方向 */
	LookingDirection,
	/* 瞄准 */
	Aiming
};

/* 第一/三人称视角 */
UENUM(BlueprintType, meta = (ScriptName = "ViewMode"))
enum class EyyViewMode : uint8
{
	ThirdPerson,
	FirstPerson
};

/* 角色模型 */
UENUM(BlueprintType, meta = (ScriptName = "OverlayState"))
enum class EyyOverlayState : uint8
{
	/* 默认模型 */
	Default,
	/* 男人模型 */
	Masculine,
	/* 女性模型 */
	Feminine,
	/* 受伤模型 */
	Injured,
	/* 束手 模型*/
	HandsTied,
	/* 步枪模型 */
	Rifle,
	/* 单手枪模型 */
	PistolOneHanded,
	/* 双手枪模型 */
	PistolTwoHanded,
	/* 弓 */
	Bow,
	/* 火炬 */
	Torch,
	/* 双筒望远镜 */
	Binoculars,
	/* 盒子 */
	Box,
	/* 桶 */
	Barrel
};

/* 持久运动状态: 地上\坠落\攀爬\失去意识 */
UENUM(BlueprintType, meta = (ScriptName = "MovementState"))
enum class EyyMovementState : uint8
{
	/* 默认 */
	None,
	/* 地上 */
	Grounded,
	/* 坠落 */
	InAir,
	/* 攀爬 */
	Mantling,
	/* 布偶:失去意识 */
	Ragdoll
};

/*  运动动作 - 攀爬、翻滚。。。 */
UENUM(BlueprintType, meta = (ScriptName = "MovementAction"))
enum class EyyMovementAction : uint8
{
	/*无动作*/
	None,
	/*低攀爬*/
	LowMantle,
	/*高攀爬*/
	HighMantle,
	/*翻滚*/
	Rolling,
	/*起身*/
	GettingUp
};
class UCurveVector;
class UCurveFloat;

/* 步伐模式的速度配置 */
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
	
	float GetSpeedForGait(const EyyGait Gait) const
	{
		switch (Gait)
		{
		case EyyGait::Running:
			return RunSpeed;
		case EyyGait::Sprinting:
			return SprintSpeed;
		case EyyGait::Walking:
			return WalkSpeed;
		default:
			return RunSpeed;			
		}
	}
};




