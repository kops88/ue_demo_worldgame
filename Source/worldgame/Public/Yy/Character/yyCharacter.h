#pragma once

#include "CoreMinimal.h"
#include "Yy/Character/yyBaseCharacter.h"
#include "yyCharacter.generated.h"

class UYyCharacterManager;

UCLASS()
class WORLDGAME_API AyyCharacter : public AyyBaseCharacter
{
	GENERATED_BODY()
	
public:
	AyyCharacter(const FObjectInitializer& ObjectInitializer);
	
	FString GetCharcterUID() const { return CharUID; }
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "yy|HeldObject")
	void UpdateHeldObject();
	
	UFUNCTION(BlueprintCallable, Category = "ALS|HeldObject")
	void ClearHeldObject();
	
	UFUNCTION(BlueprintCallable, Category = "ALS|HeldObject")
	void AttachToHand(UStaticMesh* NewStaticMesh, USkeletalMesh* NewSkeletalMesh,
					  class UClass* NewAnimClass, bool bLeftHand, FVector Offset);

protected:
	virtual void Tick(float DeltaTime) override;
	
	virtual void BeginPlay() override;
	
	virtual void OnOverlayStateChanged(EyyOverlayState PreviousState) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS|HeldObject")
	void UpdateHeldObjectAnimations();
	
public:
	/* 自定义根组件 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "yy|Component")
	TObjectPtr<USceneComponent> HeldObjectRoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "yy|Component")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "yy|Component")
	TObjectPtr<UStaticMeshComponent> StaticMesh = nullptr;

private:
	/** 角色唯一ID */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FString CharUID;
	
	
};
