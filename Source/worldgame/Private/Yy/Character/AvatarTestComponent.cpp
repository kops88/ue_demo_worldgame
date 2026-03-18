#include "Yy/Character/AvatarTestComponent.h"

#include "PropertyEditorModule.h"

UAvatarTestComponent::UAvatarTestComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}


void UAvatarTestComponent::BeginPlay()
{
	Super::BeginPlay();
	SlotHiddenBones.Add(TEXT("UpperBody"), {
		TEXT("head"),
		TEXT("Thigh_L"),
		TEXT("Thigh_R"),
		TEXT("Hand_L"),
		TEXT("hand_r"),
		TEXT("ik_foot_root"),
		TEXT("ik_hand_root"),
	});
	SlotHiddenBones.Add(TEXT("LowerBody"), {TEXT("spine_01"),
		TEXT("ik_foot_root"),
		TEXT("ik_hand_root")});
	SlotHiddenBones.Add(TEXT("Head"), {TEXT("spine_01"),
		TEXT("Thigh_L"),
		TEXT("Thigh_R"),
		TEXT("ik_foot_root"),
		TEXT("ik_hand_root"),});
	SlotUnHiddenBones.Add(TEXT("Head"), {TEXT("neck_01"),
		TEXT("head"),});
	SlotHiddenBones.Add(TEXT("Hands"), {
		TEXT("spine_01"),
		TEXT("Thigh_L"),
		TEXT("Thigh_R"),
		TEXT("ik_foot_root"),
		TEXT("ik_hand_root"),});
	SlotUnHiddenBones.Add(TEXT("Hands"), {TEXT("Hand_L"),
		TEXT("hand_r"),});

	FindComponentsByTag();
	
	if (MasterMesh)
	{
		MasterMesh->SetVisibility(false);
		InitializeSlaves();
	}
}

void UAvatarTestComponent::FindComponentsByTag()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	
	TArray<UActorComponent*> MasterResults = Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MasterMesh"));
	if (MasterResults.Num() > 0)
	{
		MasterMesh = Cast<USkeletalMeshComponent>(MasterResults[0]);
	}
	else
	{
		return;
	}	
	
	// tag -> slotName
	const TArray<TPair<FName, FName>> TagToSlotMapping = {
		{TEXT("Slave_UpperBody"), TEXT("UpperBody")},
		{TEXT("Slave_LowerBody"), TEXT("LowerBody")},
		{TEXT("Slave_Head"),      TEXT("Head")},
		{TEXT("Slave_Hands"),     TEXT("Hands")},
	};
	for (const auto& [Tag, SlotName] : TagToSlotMapping)
	{
		TArray<UActorComponent*> Results = Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), Tag);
		if (Results.Num()> 0)
		{
			SlaveMap.Add(SlotName, Cast<USkeletalMeshComponent>(Results[0]));
		}
	}
}

void UAvatarTestComponent::InitializeSlaves()
{
	for (auto& [SlotName, SlaveComp] : SlaveMap)
	{
		if (!SlaveComp) continue;
		
		const TArray<FName>* BonesToHide = SlotHiddenBones.Find(SlotName);
		TArray<FName> HideBones = BonesToHide ? *BonesToHide : TArray<FName>();
		
		SetupSlave(SlotName, SlaveComp, HideBones);
	}
	
	
}

void UAvatarTestComponent::SetupSlave(FName SlotName, USkeletalMeshComponent* Slave, const TArray<FName>& BonesToHide)
{
	if (!Slave || !MasterMesh) return;
	UE_LOG(LogTemp, Warning, TEXT("[AvatarTest] SetupSlave: Slot='%s', HideBones=%d"), 
		*SlotName.ToString(), BonesToHide.Num());
	
	// slave直接拷贝master的骨骼姿态, 而不独立计算
	Slave->SetLeaderPoseComponent(MasterMesh);
	
	for (const FName& BoneName : BonesToHide)
	{
		Slave->HideBoneByName(BoneName, PBO_None);
		UE_LOG(LogTemp, Warning, TEXT("[AvatarTest]   HideBone: '%s'"), *BoneName.ToString());
	}
	// 反隐藏
	if (const TArray<FName>* BonesToUnHide = SlotUnHiddenBones.Find(SlotName))
	{
		for (const FName& BoneName : *BonesToUnHide)
		{
			Slave->UnHideBoneByName(BoneName);
			UE_LOG(LogTemp, Warning, TEXT("[AvatarTest]   UnHideBone: '%s'"), *BoneName.ToString());		}
	}
}

void UAvatarTestComponent::SwapSlotMaterial(FName SlotName, UMaterialInterface* NewMaterial)
{
	// find返回的是指向value的指针
	USkeletalMeshComponent** SlavePtr = SlaveMap.Find(SlotName);
	if (!SlavePtr || !(*SlavePtr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[AvatarTestComponent] SwapSlotMaterial: Slot '%s' not found."), *SlotName.ToString());
		return;
	}
	if (!NewMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AvatarTestComponent] SwapSlotMesh: NewMesh is null."));
		return;
	}
	USkeletalMeshComponent* SlaveComp = *SlavePtr;
	int32 NumMaterials = SlaveComp->GetNumMaterials();
	for (int32 i = 0; i < NumMaterials; i++)
	{
		SlaveComp->SetMaterial(i, NewMaterial);
	}
}

void UAvatarTestComponent::SwapSlotMesh(FName SlotName, USkeletalMesh* NewMesh)
{
	USkeletalMeshComponent** SlavePtr = SlaveMap.Find(SlotName);
	if (!SlavePtr || !(*SlavePtr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[AvatarTestComponent] SwapSlotMesh: Slot '%s' not found."), *SlotName.ToString());
		return;
	}
	if (!NewMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AvatarTestComponent] SwapSlotMesh: NewMesh is null."));
		return;
	}
	USkeletalMeshComponent* SlaveComp = *SlavePtr;
	SlaveComp->SetSkeletalMeshAsset(NewMesh);
	SlaveComp->SetLeaderPoseComponent(MasterMesh);
	
	if (const TArray<FName>* BonesToHide = SlotHiddenBones.Find(SlotName))
	{
		for (const FName& BoneName : *BonesToHide)
		{
			SlaveComp->HideBoneByName(BoneName, EPhysBodyOp::PBO_None);
		}
	}

	// 4. 反隐藏
	if (const TArray<FName>* BonesToUnHide = SlotUnHiddenBones.Find(SlotName))
	{
		for (const FName& BoneName : *BonesToUnHide)
		{
			SlaveComp->UnHideBoneByName(BoneName);
		}
	}
}


void UAvatarTestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

