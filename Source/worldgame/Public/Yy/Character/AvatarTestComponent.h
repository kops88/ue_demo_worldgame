#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AvatarTestComponent.generated.h"


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

private:
	void FindComponentsByTag();
	void InitializeSlaves();
	void SetupSlave(FName SlotName, USkeletalMeshComponent* Slave, const TArray<FName>& BonesToHide);
	
	UPROPERTY()
	USkeletalMeshComponent* MasterMesh;
	UPROPERTY()
	TMap<FName, USkeletalMeshComponent*> SlaveMap;
	TMap<FName, TArray<FName>> SlotHiddenBones;
	TMap<FName, TArray<FName>> SlotUnHiddenBones;


	
};
