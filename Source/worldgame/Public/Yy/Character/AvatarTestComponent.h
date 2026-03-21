#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StreamableManager.h"
#include "UObject/SoftObjectPtr.h"
#include "AvatarTestComponent.generated.h"

// ============ AvaLog 自动类型转换辅助 ============

/** 第一层: 将 FName/FText/FString 统一转为 FString, 其他类型原样透传 */
FORCEINLINE FString AvaAutoConvert(const FName&   V) { return V.ToString(); }
FORCEINLINE FString AvaAutoConvert(const FText&   V) { return V.ToString(); }
FORCEINLINE FString AvaAutoConvert(const FString& V) { return V; }

template<typename T>
FORCEINLINE typename TEnableIf<
	!TIsSame<typename TDecay<T>::Type, FName>::Value &&
	!TIsSame<typename TDecay<T>::Type, FText>::Value &&
	!TIsSame<typename TDecay<T>::Type, FString>::Value,
	T&&>::Type
AvaAutoConvert(T&& V) { return Forward<T>(V); }

/** 第二层: 将 FString 转为 const TCHAR*, 其他类型原样透传 */
FORCEINLINE const TCHAR* AvaToTCHAR(const FString& V) { return *V; }

template<typename T>
FORCEINLINE typename TEnableIf<!TIsSame<typename TDecay<T>::Type, FString>::Value, T&&>::Type
AvaToTCHAR(T&& V) { return Forward<T>(V); }

/** 格式化辅助: 先 AvaAutoConvert 再 AvaToTCHAR, 保证临时 FString 生命周期覆盖 Printf 调用
 *  FmtType 保留 TCHAR[] 数组类型, 以满足 FString::Printf 的静态断言 */
template<typename FmtType, typename... TArgs>
FORCEINLINE FString AvaLogFmt(const FmtType& Fmt, TArgs&&... Args)
{
	return FString::Printf(Fmt, AvaToTCHAR(AvaAutoConvert(Forward<TArgs>(Args)))...);
}

/** 无额外参数的重载, 避免 Printf 对纯文本误判 */
template<typename FmtType>
FORCEINLINE FString AvaLogFmt(const FmtType& Fmt)
{
	return FString(Fmt);
}

/** AvaLog 宏: 支持格式化, FName/FText/FString 自动转换, 无需手动 * 或 .ToString()
 *  用法: AvaLog("hello %s %d", MyFName, 42); */
#define AvaLog(Format, ...) AvaLogImpl(AvaLogFmt(TEXT(Format), ##__VA_ARGS__))

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WORLDGAME_API UAvatarTestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAvatarTestComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "Avatar")
	void SwapSlotMaterial(FName SlotName, UMaterialInterface* NewMaterial);
	UFUNCTION(BlueprintCallable, Category = "Avatar")
	void SwapSlotMesh(FName SlotName, USkeletalMesh* NewMesh);

protected:
	virtual void BeginPlay() override;

	/** 设置SlaveMesh, 异步 */
	UFUNCTION(BlueprintCallable)
	void SetSlaveMesh(FName SlotName, TSoftObjectPtr<USkeletalMesh> Mesh);

private:

	/** 有无装载Mesh到slave */
	bool IsLoadMesh = false; 

	/** 每个Slot对应的默认SkeletalMesh软引用, 在蓝图/编辑器中配置路径 slotNmae== UpperBody*/
	UPROPERTY(EditAnywhere, Category = "Avatar|DefaultMesh")
	TMap<FName, TSoftObjectPtr<USkeletalMesh>> DefaultMeshMap;

	/** 异步加载句柄, 防止加载过程中被GC */
	TSharedPtr<FStreamableHandle> MeshStreamableHandle;

	/** 异步加载管理器 */
	FStreamableManager StreamableManager;

	/** 异步加载完成回调 */
	void OnMeshLoaded(TMap<FName, TSoftObjectPtr<USkeletalMesh>>& MeshMap);

	void FindComponentsByTag();
	void InitializeSlaves();
	void SetupSlave(FName SlotName, USkeletalMeshComponent* Slave, const TArray<FName>& BonesToHide);
	UFUNCTION()
	void SetDefaultMesh(); 

	void AvaLogImpl(const FString& Msg);
	
	UPROPERTY()
	USkeletalMeshComponent* MasterMesh;
	UPROPERTY()
	TMap<FName, USkeletalMeshComponent*> SlaveMap;
	TMap<FName, TArray<FName>> SlotHiddenBones;
	TMap<FName, TArray<FName>> SlotUnHiddenBones;


	
};
