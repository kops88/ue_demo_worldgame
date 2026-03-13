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
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Breakfall System")
	EyyGroundedEntryState State = EyyGroundedEntryState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Breakfall System")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Breakfall System")
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
















