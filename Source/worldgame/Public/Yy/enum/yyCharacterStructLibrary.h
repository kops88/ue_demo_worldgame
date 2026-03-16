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

UENUM(BlueprintType, meta = (ScriptName = "ALS_GroundedEntryState"))
enum class EyyGroundedEntryState : uint8
{
	None,
	Roll
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

/* 
 * 持久运动状态: 地上\坠落\攀爬\失去意识
 * 还包括各个状态的bool值
 */
USTRUCT(BlueprintType)
struct FyyMovementState
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	EyyMovementState State = EyyMovementState::None;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool Grounded_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool InAir_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool Mantling_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool Ragdoll_ = false;

public:
	FyyMovementState()
	{
	}

	FyyMovementState(const EyyMovementState InitialState) { *this = InitialState; }
	const bool& None() const { return None_; }
	const bool& Grounded() const { return Grounded_; }
	const bool& InAir() const { return InAir_; }
	const bool& Mantling() const { return Mantling_; }
	const bool& Ragdoll() const { return Ragdoll_; }

	operator EyyMovementState() const { return State; }

	void operator=(const EyyMovementState NewState)
	{
		State = NewState;
		None_ = State == EyyMovementState::None;
		Grounded_ = State == EyyMovementState::Grounded;
		InAir_ = State == EyyMovementState::InAir;
		Mantling_ = State == EyyMovementState::Mantling;
		Ragdoll_ = State == EyyMovementState::Ragdoll;
	}
};

USTRUCT(BlueprintType)
struct FyyMovementAction
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	EyyMovementAction Action = EyyMovementAction::None;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool None_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool LowMantle_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool HighMantle_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool Rolling_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool GettingUp_ = false;

public:
	FyyMovementAction()
	{
	}

	FyyMovementAction(const EyyMovementAction InitialAction) { *this = InitialAction; }
	const bool& None() const { return None_; }
	const bool& LowMantle() const { return LowMantle_; }
	const bool& HighMantle() const { return HighMantle_; }
	const bool& Rolling() const { return Rolling_; }
	const bool& GettingUp() const { return GettingUp_; }

	operator EyyMovementAction() const { return Action; }

	void operator=(const EyyMovementAction NewAction)
	{
		Action = NewAction;
		None_ = Action == EyyMovementAction::None;
		LowMantle_ = Action == EyyMovementAction::LowMantle;
		HighMantle_ = Action == EyyMovementAction::HighMantle;
		Rolling_ = Action == EyyMovementAction::Rolling;
		GettingUp_ = Action == EyyMovementAction::GettingUp;
	}
};

USTRUCT(BlueprintType)
struct FyyStance
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	EyyStance Stance = EyyStance::Standing;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool Standing_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool Crouching_ = false;

public:
	FyyStance()
	{
	}

	FyyStance(const EyyStance InitialStance) { *this = InitialStance; }
	const bool& Standing() const { return Standing_; }
	const bool& Crouching() const { return Crouching_; }

	operator EyyStance() const { return Stance; }

	void operator=(const EyyStance NewStance)
	{
		Stance = NewStance;
		Standing_ = Stance == EyyStance::Standing;
		Crouching_ = Stance == EyyStance::Crouching;
	}
};

/* 角色朝向模式 */
USTRUCT(BlueprintType)
struct FyyRotationMode
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Rotation System")
	EyyRotationMode RotationMode = EyyRotationMode::VelocityDirection;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Rotation System")
	bool VelocityDirection_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Rotation System")
	bool LookingDirection_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Rotation System")
	bool Aiming_ = false;

public:
	FyyRotationMode()
	{
	}

	FyyRotationMode(const EyyRotationMode InitialRotationMode) { *this = InitialRotationMode; }
	const bool& VelocityDirection() const { return VelocityDirection_; }
	const bool& LookingDirection() const { return LookingDirection_; }
	const bool& Aiming() const { return Aiming_; }

	operator EyyRotationMode() const { return RotationMode; }

	void operator=(const EyyRotationMode NewRotationMode)
	{
		RotationMode = NewRotationMode;
		VelocityDirection_ = RotationMode == EyyRotationMode::VelocityDirection;
		LookingDirection_ = RotationMode == EyyRotationMode::LookingDirection;
		Aiming_ = RotationMode == EyyRotationMode::Aiming;
	}
};

USTRUCT(BlueprintType)
struct FyyGait
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	EyyGait Gait = EyyGait::Walking;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool Walking_ = true;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool Running_ = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Movement System")
	bool Sprinting_ = false;
	
public:
	FyyGait()
	{
	}
	
	FyyGait(const EyyGait InitialGait) { *this = InitialGait; }
	const bool& Walking() const { return Walking_; }
	const bool& Running() const { return Running_; }
	const bool& Sprinting() const { return Sprinting_; }
	
	operator EyyGait() const { return Gait; }
	
	void operator=(const EyyGait NewGait)
	{
		Gait = NewGait;
		Walking_ = Gait == EyyGait::Walking;
		Running_ = Gait == EyyGait::Running;
		Sprinting_ = Gait == EyyGait::Sprinting;
	}
};

USTRUCT(BlueprintType)
struct FyyOverlayState
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	EyyOverlayState State = EyyOverlayState::Default;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool Default_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool Masculine_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool Feminine_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool Injured_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool HandsTied_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool Rifle_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool PistolOneHanded_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool PistolTwoHanded_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool Bow_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool Torch_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool Binoculars_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool Box_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Character States")
	bool Barrel_ = false;

public:
	FyyOverlayState()
	{
	}

	FyyOverlayState(const EyyOverlayState InitialState) { *this = InitialState; }
	const bool& Default() const { return Default_; }
	const bool& Masculine() const { return Masculine_; }
	const bool& Feminine() const { return Feminine_; }
	const bool& Injured() const { return Injured_; }
	const bool& HandsTied() const { return HandsTied_; }
	const bool& Rifle() const { return Rifle_; }
	const bool& PistolOneHanded() const { return PistolOneHanded_; }
	const bool& PistolTwoHanded() const { return PistolTwoHanded_; }
	const bool& Bow() const { return Bow_; }
	const bool& Torch() const { return Torch_; }
	const bool& Binoculars() const { return Binoculars_; }
	const bool& Box() const { return Box_; }
	const bool& Barrel() const { return Barrel_; }

	operator EyyOverlayState() const { return State; }

	void operator=(const EyyOverlayState NewState)
	{
		State = NewState;
		Default_ = State == EyyOverlayState::Default;
		Masculine_ = State == EyyOverlayState::Masculine;
		Feminine_ = State == EyyOverlayState::Feminine;
		Injured_ = State == EyyOverlayState::Injured;
		HandsTied_ = State == EyyOverlayState::HandsTied;
		Rifle_ = State == EyyOverlayState::Rifle;
		PistolOneHanded_ = State == EyyOverlayState::PistolOneHanded;
		PistolTwoHanded_ = State == EyyOverlayState::PistolTwoHanded;
		Bow_ = State == EyyOverlayState::Bow;
		Torch_ = State == EyyOverlayState::Torch;
		Binoculars_ = State == EyyOverlayState::Binoculars;
		Box_ = State == EyyOverlayState::Box;
		Barrel_ = State == EyyOverlayState::Barrel;
	}
};

USTRUCT(BlueprintType)
struct FyyGroundedEntryState
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Breakfall System")
	EyyGroundedEntryState State = EyyGroundedEntryState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Breakfall System")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Breakfall System")
	bool Roll_ = false;

public:
	FyyGroundedEntryState()
	{
	}

	FyyGroundedEntryState(const EyyGroundedEntryState InitialState) { *this = InitialState; }

	const bool& None() const { return None_; }
	const bool& Roll() const { return Roll_; }

	operator EyyGroundedEntryState() const { return State; }

	void operator=(const EyyGroundedEntryState NewState)
	{
		State = NewState;
		None_ = State == EyyGroundedEntryState::None;
		Roll_ = State == EyyGroundedEntryState::Roll;
	}
};

USTRUCT(BlueprintType)
struct FyyAnimGraphAimingValues
{
	GENERATED_BODY()
	
	/*平滑后的瞄准旋转角度*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Aiming Values")
	FRotator SmoothedAimingRotation = FRotator::ZeroRotator;
	/*脊柱旋转角度*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Aiming Values")
	FRotator SpineRotation = FRotator::ZeroRotator;
	/*瞄准角度（二维向量）*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Aiming Values")
	FVector2D AimingAngle = FVector2D::ZeroVector;
	/*瞄准扫描时间*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Aiming Values")
	float AimSweepTime = 0.5f;
	/*输入偏航偏移时间:记录输入方向与当前朝向之间的时间偏移，用于处理角色转向的延迟*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Aiming Values")
	float InputYawOffsetTime = 0.0f;
	/*前向偏航时间:控制角色向前瞄准的时间参数*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Aiming Values")
	float ForwardYawTime = 0.0f;
	/*左偏航时间:控制角色向左瞄准的时间参数*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Aiming Values")
	float LeftYawTime = 0.0f;
	/**/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Aiming Values")
	float RightYawTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FyyAnimConfiguration
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float AnimatedWalkSpeed = 150.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float AnimatedRunSpeed = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float AnimatedSprintSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float AnimatedCrouchSpeed = 150.0f;

	/* 速度混合插值速度 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float VelocityBlendInterpSpeed = 12.0f;

	/* 地面倾斜插值速度*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float GroundedLeanInterpSpeed = 4.0f;

	/*空中倾斜插值速度*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float InAirLeanInterpSpeed = 4.0f;

	/*平滑瞄准旋转插值速度*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float SmoothedAimingRotationInterpSpeed = 10.0f;

	/*输入偏航偏移插值速度*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float InputYawOffsetInterpSpeed = 8.0f;

	/*当角色速度低于这个值时，可以触发转身动画而不是直接转向*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float TriggerPivotSpeedLimit = 200.0f;
	
	/*用于IK（反向运动学）计算，确定脚部与地面的距离*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float FootHeight = 13.5f;

	/** 动态过渡动画的激活阈值:控制何时从一种动画状态平滑过渡到另一种状态 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float DynamicTransitionThreshold = 8.0f;

	/*脚部上方IK追踪距离 : 用于IK系统的射线追踪，检测脚部上方的碰撞 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float IK_TraceDistanceAboveFoot = 50.0f;
	/*脚部下方IK追踪距离 : 用于IK系统的射线追踪，检测脚部下方的碰撞 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Configuration")
	float IK_TraceDistanceBelowFoot = 45.0f;
};


/* 用于设置FootIK的各种参数*/
USTRUCT(BlueprintType)
struct FyyAnimGraphFootIK
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	float FootLock_L_Alpha = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	float FootLock_R_Alpha = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	bool UseFootLockCurve_L = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	bool UseFootLockCurve_R = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FVector FootLock_L_Location = FVector::ZeroVector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FVector TargetFootLock_R_Location = FVector::ZeroVector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FVector FootLock_R_Location = FVector::ZeroVector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FRotator TargetFootLock_L_Rotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FRotator FootLock_L_Rotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FRotator TargetFootLock_R_Rotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FRotator FootLock_R_Rotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FVector FootOffset_L_Location = FVector::ZeroVector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FVector FootOffset_R_Location = FVector::ZeroVector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FRotator FootOffset_L_Rotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FRotator FootOffset_R_Rotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	FVector PelvisOffset = FVector::ZeroVector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Foot IK")
	float PelvisAlpha = 0.0f;
};

/*  角色髋部方向控制器 */
UENUM(BlueprintType, meta = (ScriptName = "ALS_HipsDirection"))
enum class EyyHipsDirection : uint8
{
	F,
	B,
	RF,
	RB,
	LF,
	LB
};


USTRUCT(BlueprintType)
struct FyyAnimGraphGrounded
{
	GENERATED_BODY()
	
	/* base layer - locomotion states - n stop - n stop states - plant left foot 状态的 混合方向枚举*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Anim Graph - Grounded")
	EyyHipsDirection TrackedHipsDirection = EyyHipsDirection::F;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	bool bShouldMove = false; // Should be false initially

	/* baselayer n - locomotion states - n not moving - n rotate left 90 转换规则*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	bool bRotateL = false;

	/* baselayer n - locomotion states - n not moving - n rotate right 90 转换规则*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	bool bRotateR = false;

	 /* 
	  * 状态机 base layer - n locomotion detail - n runing -> first pivot 的规则
	  */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Anim Graph - Grounded")
	bool bPivot = false;

	/* 播放动画序列, 直接控制playRate参数: ALS_N_Rotate_L90*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	float RotateRate = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	float RotationScale = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	float DiagonalScaleAmount = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	float WalkRunBlend = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	float StandingPlayRate = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	float CrouchingPlayRate = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	float StrideBlend = 0.0f;

	/* 曲线 cycleBlending 中的 move f 状态*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	float FYaw = 0.0f;

	/* 曲线*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	float BYaw = 0.0f;

	/* 曲线*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	float LYaw = 0.0f;

	/* 曲线*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Anim Graph - Grounded")
	float RYaw = 0.0f;
};

/*单个转身动画资源包*/
USTRUCT(BlueprintType)
struct FyyTurnInPlaceAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Turn In Place")
	TObjectPtr<UAnimSequenceBase> Animation = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Turn In Place")
	float AnimatedAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Turn In Place")
	FName SlotName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Turn In Place")
	float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Turn In Place")
	bool ScaleTurnAngle = true;
};




/* 角色原地转身动画控制器 */
USTRUCT(BlueprintType)
struct FyyAnimTurnInPlace
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	float TurnCheckMinAngle = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	float Turn180Threshold = 130.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	float AimYawRateLimit = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	float ElapsedDelayTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	float MinAngleDelay = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	float MaxAngleDelay = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	FyyTurnInPlaceAsset N_TurnIP_L90;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	FyyTurnInPlaceAsset N_TurnIP_R90;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	FyyTurnInPlaceAsset N_TurnIP_L180;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	FyyTurnInPlaceAsset N_TurnIP_R180;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	FyyTurnInPlaceAsset CLF_TurnIP_L90;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	FyyTurnInPlaceAsset CLF_TurnIP_R90;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	FyyTurnInPlaceAsset CLF_TurnIP_L180;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	FyyTurnInPlaceAsset CLF_TurnIP_R180;
};


/*当前速度在每个方向上的分量比例，用于驱动多方向移动动画的混合*/
USTRUCT(BlueprintType)
struct FyyVelocityBlend
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Grounded")
	float F = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Grounded")
	float B = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Grounded")
	float L = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Grounded")
	float R = 0.0f;
};

/*身体倾斜量, 范围-1.0 ~ 1.0*/
USTRUCT(BlueprintType)
struct FyyLeanAmount
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Grounded")
	float LR = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "ALS|Anim Graph - Grounded")
	float FB = 0.0f;
};


UENUM(BlueprintType, meta = (ScriptName = "ALS_MovementDirection"))
enum class EyyMovementDirection : uint8
{
	Forward,
	Right,
	Left,
	Backward
};

USTRUCT(BlueprintType)
struct FyyMovementDirection
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	EyyMovementDirection Direction = EyyMovementDirection::Forward;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Forward_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Right_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Left_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Backward_ = false;

public:
	FyyMovementDirection()
	{
	}

	FyyMovementDirection(const EyyMovementDirection InitialMovementDirection)
	{
		*this = InitialMovementDirection;
	}

	const bool& Forward() const { return Forward_; }
	const bool& Right() const { return Right_; }
	const bool& Left() const { return Left_; }
	const bool& Backward() const { return Backward_; }

	operator EyyMovementDirection() const { return Direction; }

	void operator=(const EyyMovementDirection NewMovementDirection)
	{
		Direction = NewMovementDirection;
		Forward_ = Direction == EyyMovementDirection::Forward;
		Right_ = Direction == EyyMovementDirection::Right;
		Left_ = Direction == EyyMovementDirection::Left;
		Backward_ = Direction == EyyMovementDirection::Backward;
	}

	
};

USTRUCT(BlueprintType)
struct FyyAnimRotateInPlace
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Rotate In Place")
	float RotateMinThreshold = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Rotate In Place")
	float RotateMaxThreshold = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Rotate In Place")
	float AimYawRateMinRange = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Rotate In Place")
	float AimYawRateMaxRange = 270.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Rotate In Place")
	float MinPlayRate = 1.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Rotate In Place")
	float MaxPlayRate = 3.0f;
};

USTRUCT(BlueprintType)
struct FyyDynamicMontageParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Dynamic Transition")
	TObjectPtr<UAnimSequenceBase> Animation = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Dynamic Transition")
	float BlendInTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Dynamic Transition")
	float BlendOutTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Dynamic Transition")
	float PlayRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Dynamic Transition")
	float StartTime = 0.0f;
};



























