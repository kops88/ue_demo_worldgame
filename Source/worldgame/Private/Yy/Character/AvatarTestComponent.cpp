#include "Yy/Character/AvatarTestComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "PropertyEditorModule.h"

// AvaLog 宏已在头文件中定义, 支持 FName/FText/FString 自动转换

UAvatarTestComponent::UAvatarTestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAvatarTestComponent::BeginPlay()
{
	Super::BeginPlay();
	AvaLog("BeginPlay Start");
	FindComponentsByTag();
	SetDefaultMesh();
	if (MasterMesh)
	{
		// 隐藏 Master 的渲染，但保持动画求值正常运行
		MasterMesh->SetHiddenInGame(true);
		// 强制 Master 即使不可见也要每帧计算动画
		MasterMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		InitializeSlaves();
	}
}

void UAvatarTestComponent::SetSlaveMesh(FName SlotName, TSoftObjectPtr<USkeletalMesh> Mesh)
{
	if(!Mesh.IsValid())
	{
		AvaLog("SetSlaveMesh() Mesh is null.");
		return;
	}
	TMap<FName, TSoftObjectPtr<USkeletalMesh>> TempMap;
	TempMap.Add(SlotName, Mesh);
	TArray<FSoftObjectPath> AssetsToLoad;
	AssetsToLoad.Add(Mesh.ToSoftObjectPath());
	MeshStreamableHandle = StreamableManager.RequestAsyncLoad(
		AssetsToLoad,
		[this, TempMap = MoveTemp(TempMap)]() mutable { OnMeshLoaded(TempMap); }
	);
	AvaLog("SetSlaveMesh() 开始异步加载 Mesh 资源...");
}

void UAvatarTestComponent::SetDefaultMesh()
{
	if (DefaultMeshMap.Num() == 0)
	{
		AvaLog("SetDefaultMesh() DefaultMeshMap 为空，请在编辑器中配置。");
		return;
	}

	// 收集所有需要加载的软引用路径
	TArray<FSoftObjectPath> AssetsToLoad;
	for (const auto& [SlotName, SoftMesh] : DefaultMeshMap)
	{
		if (!SoftMesh.IsNull())
		{
			AssetsToLoad.Add(SoftMesh.ToSoftObjectPath());
		}
	}

	if (AssetsToLoad.Num() == 0)
	{
		AvaLog("SetDefaultMesh() 没有有效的Mesh路径需要加载。");
		return;
	}

	// 使用 StreamableManager 批量异步加载
	MeshStreamableHandle = StreamableManager.RequestAsyncLoad(
		AssetsToLoad,
		[this]() { OnMeshLoaded(DefaultMeshMap); }
	);

	AvaLog("SetDefaultMesh() 开始异步加载 %d 个Mesh资源...", AssetsToLoad.Num());
}


void UAvatarTestComponent::OnMeshLoaded(TMap<FName, TSoftObjectPtr<USkeletalMesh>>& MeshMap)
{
	AvaLog("OnDefaultMeshLoaded() 异步加载完成。");

	for (const auto& [SlotName, SoftMesh] : MeshMap)
	{
		if (SoftMesh.IsNull()) continue;

		// 加载完成后 Get() 即可获取已加载的资源
		USkeletalMesh* LoadedMesh = SoftMesh.Get();
		if (!LoadedMesh)
		{
			AvaLog("OnDefaultMeshLoaded() Slot '%s' 的 Mesh 加载失败。", SlotName);
			continue;
		}

		// 找到对应的 SlaveComp 并设置 Mesh
		USkeletalMeshComponent** SlavePtr = SlaveMap.Find(SlotName);
		if (!SlavePtr || !(*SlavePtr))
		{
			AvaLog("OnDefaultMeshLoaded() Slot '%s' 对应的 SlaveComp 未找到。", SlotName);
			continue;
		}

		USkeletalMeshComponent* SlaveComp = *SlavePtr;
		SlaveComp->SetSkeletalMeshAsset(LoadedMesh);
		SlaveComp->SetLeaderPoseComponent(MasterMesh);

		AvaLog("OnDefaultMeshLoaded() Slot '%s' 已设置 Mesh '%s'。", SlotName, LoadedMesh->GetName());
	}

	IsLoadMesh = true;
}

/** 找到对应的组件-> MasterMesh, SlaveMap */
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
	
	// tag -> slotName -> SlaveComp, 在验证版中, 不代表实际的slot插槽
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

/** 初始化; 现在仅包括: 设置LeaderPoseComponent */
void UAvatarTestComponent::InitializeSlaves()
{
	for (auto& [SlotName, SlaveComp] : SlaveMap)
	{
		if (!SlaveComp) {
			AvaLog("InitializeSlaves(), SlaveComp: %s is null", SlotName);
			continue;
		};

		SlaveComp->SetLeaderPoseComponent(MasterMesh);
	}
}

/** 可以废弃了 */
void UAvatarTestComponent::SetupSlave(FName SlotName, USkeletalMeshComponent* Slave, const TArray<FName>& BonesToHide)
{
	if (!Slave || !MasterMesh) return;
	AvaLog("SetupSlave() Slot='%s', HideBones=%d", SlotName, BonesToHide.Num());
	
	Slave->SetLeaderPoseComponent(MasterMesh);
}

void UAvatarTestComponent::SwapSlotMaterial(FName SlotName, UMaterialInterface* NewMaterial)
{
	// find返回的是指向value的指针
	USkeletalMeshComponent** SlavePtr = SlaveMap.Find(SlotName);
	if (!SlavePtr || !(*SlavePtr))
	{
		AvaLog("SwapSlotMaterial() Slot '%s' not found.", SlotName);
		return;
	}
	if (!NewMaterial)
	{
		AvaLog("SwapSlotMaterial() NewMaterial is null.");
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
		AvaLog("SwapSlotMesh() Slot '%s' not found.", SlotName);
		return;
	}
	if (!NewMesh)
	{
		AvaLog("SwapSlotMesh() NewMesh is null.");
		return;
	}
	USkeletalMeshComponent* SlaveComp = *SlavePtr;
	SlaveComp->SetSkeletalMeshAsset(NewMesh);
	SlaveComp->SetLeaderPoseComponent(MasterMesh);
}


void UAvatarTestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}







void UAvatarTestComponent::AvaLogImpl(const FString& Msg)
{
	UE_LOG(LogTemp, Log, TEXT("[AvatarTestComponent] %s"), *Msg);
}











