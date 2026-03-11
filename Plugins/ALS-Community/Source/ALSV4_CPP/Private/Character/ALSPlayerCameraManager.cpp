// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/ALSPlayerCameraManager.h"


#include "Character/ALSBaseCharacter.h"
#include "Character/ALSPlayerController.h"
#include "Character/Animation/ALSPlayerCameraBehavior.h"
#include "Components/ALSDebugComponent.h"

#include "Kismet/KismetMathLibrary.h"


const FName NAME_CameraBehavior(TEXT("CameraBehavior"));
const FName NAME_CameraOffset_X(TEXT("CameraOffset_X"));
const FName NAME_CameraOffset_Y(TEXT("CameraOffset_Y"));
const FName NAME_CameraOffset_Z(TEXT("CameraOffset_Z"));
const FName NAME_Override_Debug(TEXT("Override_Debug"));
const FName NAME_PivotLagSpeed_X(TEXT("PivotLagSpeed_X"));
const FName NAME_PivotLagSpeed_Y(TEXT("PivotLagSpeed_Y"));
const FName NAME_PivotLagSpeed_Z(TEXT("PivotLagSpeed_Z"));
const FName NAME_PivotOffset_X(TEXT("PivotOffset_X"));
const FName NAME_PivotOffset_Y(TEXT("PivotOffset_Y"));
const FName NAME_PivotOffset_Z(TEXT("PivotOffset_Z"));
const FName NAME_RotationLagSpeed(TEXT("RotationLagSpeed"));
const FName NAME_Weight_FirstPerson(TEXT("Weight_FirstPerson"));


AALSPlayerCameraManager::AALSPlayerCameraManager()
{
	CameraBehavior = CreateDefaultSubobject<USkeletalMeshComponent>(NAME_CameraBehavior);
	CameraBehavior->SetupAttachment(GetRootComponent());
	CameraBehavior->bHiddenInGame = true;
}

void AALSPlayerCameraManager::OnPossess(AALSBaseCharacter* NewCharacter)
{
	// Set "Controlled Pawn" when Player Controller Possesses new character. (called from Player Controller)
	check(NewCharacter);
	ControlledCharacter = NewCharacter;

	// Update references in the Camera Behavior AnimBP.
	UALSPlayerCameraBehavior* CastedBehv = Cast<UALSPlayerCameraBehavior>(CameraBehavior->GetAnimInstance());
	if (CastedBehv)
	{
		NewCharacter->SetCameraBehavior(CastedBehv);
		CastedBehv->MovementState = NewCharacter->GetMovementState();
		CastedBehv->MovementAction = NewCharacter->GetMovementAction();
		CastedBehv->bRightShoulder = NewCharacter->IsRightShoulder();
		CastedBehv->Gait = NewCharacter->GetGait();
		CastedBehv->SetRotationMode(NewCharacter->GetRotationMode());
		CastedBehv->Stance = NewCharacter->GetStance();
		CastedBehv->ViewMode = NewCharacter->GetViewMode();
	}

	// Initial position
	const FVector& TPSLoc = ControlledCharacter->GetThirdPersonPivotTarget().GetLocation();
	SetActorLocation(TPSLoc);
	SmoothedPivotTarget.SetLocation(TPSLoc);

	ALSDebugComponent = ControlledCharacter->FindComponentByClass<UALSDebugComponent>();
}

float AALSPlayerCameraManager::GetCameraBehaviorParam(FName CurveName) const
{
	UAnimInstance* Inst = CameraBehavior->GetAnimInstance();
	if (Inst)
	{
		return Inst->GetCurveValue(CurveName);
	}
	return 0.0f;
}

void AALSPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	// Partially taken from base class

	if (OutVT.Target)
	{
		FVector OutLocation;
		FRotator OutRotation;
		float OutFOV;

		if (OutVT.Target->IsA<AALSBaseCharacter>())
		{
			if (CustomCameraBehavior(DeltaTime, OutLocation, OutRotation, OutFOV))
			{
				OutVT.POV.Location = OutLocation;
				OutVT.POV.Rotation = OutRotation;
				OutVT.POV.FOV = OutFOV;
			}
			else
			{
				OutVT.Target->CalcCamera(DeltaTime, OutVT.POV);
			}
		}
		else
		{
			OutVT.Target->CalcCamera(DeltaTime, OutVT.POV);
		}
	}
}

FVector AALSPlayerCameraManager::CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation,
                                                             FRotator CameraRotation, FVector LagSpeeds,
                                                             float DeltaTime)
{
	CameraRotation.Roll = 0.0f;
	CameraRotation.Pitch = 0.0f;
	const FVector UnrotatedCurLoc = CameraRotation.UnrotateVector(CurrentLocation);
	const FVector UnrotatedTargetLoc = CameraRotation.UnrotateVector(TargetLocation);

	const FVector ResultVector(
		FMath::FInterpTo(UnrotatedCurLoc.X, UnrotatedTargetLoc.X, DeltaTime, LagSpeeds.X),
		FMath::FInterpTo(UnrotatedCurLoc.Y, UnrotatedTargetLoc.Y, DeltaTime, LagSpeeds.Y),
		FMath::FInterpTo(UnrotatedCurLoc.Z, UnrotatedTargetLoc.Z, DeltaTime, LagSpeeds.Z));

	return CameraRotation.RotateVector(ResultVector);
}

bool AALSPlayerCameraManager::CustomCameraBehavior(float DeltaTime, FVector& Location, FRotator& Rotation, float& FOV)
{
	if (!ControlledCharacter)
	{
		return false;
	}

	/*
	 * PivotTarget 绿球 角色骨骼上的真实位置，每帧瞬间更新
	 * SmoothedPivotTarget 橙球 慢慢追绿球的中间点，有延迟感
	 * PivotLocation 蓝球 橙球 + 偏移量（比如肩膀偏移）
	 * TargetCameraLocation 摄像机 蓝球 + 摄像机相对偏移，这才是最终摄像机位置
	 */

	/* 获取人物枢纽坐标(绿球)**/
	// Step 1: Get Camera Parameters from CharacterBP via the Camera Interface
	const FTransform& PivotTarget = ControlledCharacter->GetThirdPersonPivotTarget();
	/* 获取第一人称位置: 眼睛*/
	const FVector& FPTarget = ControlledCharacter->GetFirstPersonCameraTarget();
	float TPFOV = 90.0f;
	float FPFOV = 90.0f;
	bool bRightShoulder = false;
	/* 获取FOV 视野广度 左右肩*/
	ControlledCharacter->GetCameraParameters(TPFOV, FPFOV, bRightShoulder);

	/* 平滑过度的旋转: 从摄像机当前旋转到控制器旋转*/
	/* GetCameraBehaviorParam 读取曲线当前状态的值是多少 */
	// Step 2: Calculate Target Camera Rotation. Use the Control Rotation and interpolate for smooth camera rotation.
	const FRotator& InterpResult = FMath::RInterpTo(GetCameraRotation(),
	                                                GetOwningPlayerController()->GetControlRotation(), DeltaTime,
	                                                GetCameraBehaviorParam(NAME_RotationLagSpeed));
	/* 摄像机旋转*/
	TargetCameraRotation = UKismetMathLibrary::RLerp(InterpResult, DebugViewRotation,
	                                                 GetCameraBehaviorParam(TEXT("Override_Debug")), true);

	/* 获取摄像机平滑过度的不同方向的速度*/
	// Step 3: Calculate the Smoothed Pivot Target (Orange Sphere).
	// Get the 3P Pivot Target (Green Sphere) and interpolate using axis independent lag for maximum control.
	const FVector LagSpd(GetCameraBehaviorParam(NAME_PivotLagSpeed_X),
	                     GetCameraBehaviorParam(NAME_PivotLagSpeed_Y),
	                     GetCameraBehaviorParam(NAME_PivotLagSpeed_Z));

	/* 获取橙色球在该帧的新位置(根据t插值)*/
	const FVector& AxisIndpLag = CalculateAxisIndependentLag(SmoothedPivotTarget.GetLocation(),
	                                                         PivotTarget.GetLocation(), TargetCameraRotation, LagSpd,
	                                                         DeltaTime);

	/*橙色球体 = 插值的位置 + 直接复制的旋转, 用于平滑位置的过度*/
	SmoothedPivotTarget.SetRotation(PivotTarget.GetRotation());
	SmoothedPivotTarget.SetLocation(AxisIndpLag);
	SmoothedPivotTarget.SetScale3D(FVector::OneVector);

	/* 蓝球是在橙球基础上，沿橙球自身朝向做了一个位置偏移的高度, 如高度, 肩膀左右的偏移 */
	/* 篮球的NAME_PivotOffset_Z可能是主要高度控制来源*/
	// Step 4: Calculate Pivot Location (BlueSphere). Get the Smoothed
	// Pivot Target and apply local offsets for further camera control.
	PivotLocation =
		SmoothedPivotTarget.GetLocation() +
		UKismetMathLibrary::GetForwardVector(SmoothedPivotTarget.Rotator()) * GetCameraBehaviorParam(
			NAME_PivotOffset_X) +
		UKismetMathLibrary::GetRightVector(SmoothedPivotTarget.Rotator()) * GetCameraBehaviorParam(
			NAME_PivotOffset_Y) +
		UKismetMathLibrary::GetUpVector(SmoothedPivotTarget.Rotator()) * GetCameraBehaviorParam(
			NAME_PivotOffset_Z);

	
	/*从蓝色球出发，再往摄像机方向推一段距离，得到摄像机的目标位置。*/
	/* 如果在NAME_CameraOffset_X设置为负数, 则可以让摄像机在玩家后面*/
	// Step 5: Calculate Target Camera Location. Get the Pivot location and apply camera relative offsets.
	TargetCameraLocation = UKismetMathLibrary::VLerp(
		PivotLocation +
		UKismetMathLibrary::GetForwardVector(TargetCameraRotation) * GetCameraBehaviorParam(
			NAME_CameraOffset_X) +
		UKismetMathLibrary::GetRightVector(TargetCameraRotation) * GetCameraBehaviorParam(NAME_CameraOffset_Y)
		+
		UKismetMathLibrary::GetUpVector(TargetCameraRotation) * GetCameraBehaviorParam(NAME_CameraOffset_Z),
		PivotTarget.GetLocation() + DebugViewOffset,
		GetCameraBehaviorParam(NAME_Override_Debug));

	
	/* 障碍物检测, 如果中间有障碍物, 拉倒障碍物前*/
	// Step 6: Trace for an object between the camera and character to apply a corrective offset.
	// Trace origins are set within the Character BP via the Camera Interface.
	// Functions like the normal spring arm, but can allow for different trace origins regardless of the pivot
	FVector TraceOrigin;
	float TraceRadius;
	ECollisionChannel TraceChannel = ControlledCharacter->GetThirdPersonTraceParams(TraceOrigin, TraceRadius);

	UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(ControlledCharacter);

	FHitResult HitResult;
	const FCollisionShape SphereCollisionShape = FCollisionShape::MakeSphere(TraceRadius);
	const bool bHit = World->SweepSingleByChannel(HitResult, TraceOrigin, TargetCameraLocation, FQuat::Identity,
	                                              TraceChannel, SphereCollisionShape, Params);

	if (ALSDebugComponent && ALSDebugComponent->GetShowTraces())
	{
		UALSDebugComponent::DrawDebugSphereTraceSingle(World,
		                                               TraceOrigin,
		                                               TargetCameraLocation,
		                                               SphereCollisionShape,
		                                               EDrawDebugTrace::Type::ForOneFrame,
		                                               bHit,
		                                               HitResult,
		                                               FLinearColor::Red,
		                                               FLinearColor::Green,
		                                               5.0f);
	}

	if (HitResult.IsValidBlockingHit())
	{
		/* 将摄像机位置拉到障碍物前 */
		TargetCameraLocation += HitResult.Location - HitResult.TraceEnd;
	}

	 /* 在第一人称和第三人称之间平滑过度*/
	/* NAME_Weight_FirstPerson 0 是第一人称, 1 是第三人称*/
	// Step 8: Lerp First Person Override and return target camera parameters.
	FTransform TargetCameraTransform(TargetCameraRotation, TargetCameraLocation, FVector::OneVector);
	FTransform FPTargetCameraTransform(TargetCameraRotation, FPTarget, FVector::OneVector);
/* 在两个值 A 和 B 之间根据一个比例（Alpha，通常是0~1之间的浮点数）计算中间值。 */
	const FTransform& MixedTransform = UKismetMathLibrary::TLerp(TargetCameraTransform, FPTargetCameraTransform,
	                                                             GetCameraBehaviorParam(
		                                                             NAME_Weight_FirstPerson));

	const FTransform& TargetTransform = UKismetMathLibrary::TLerp(MixedTransform,
	                                                              FTransform(DebugViewRotation, TargetCameraLocation,
	                                                                         FVector::OneVector),
	                                                              GetCameraBehaviorParam(
		                                                              NAME_Override_Debug));

	/* 输出 */
	Location = TargetTransform.GetLocation();
	Rotation = TargetTransform.Rotator();
	FOV = FMath::Lerp(TPFOV, FPFOV, GetCameraBehaviorParam(NAME_Weight_FirstPerson));

	return true;
}
