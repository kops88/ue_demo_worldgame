// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "YyCharacterManager.generated.h"

class AyyCharacter;

/**
 * 角色管理器 - 管理所有 AyyCharacter 实例
 */
UCLASS()
class WORLDGAME_API UYyCharacterManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UYyCharacterManager();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	/** 注册角色 */
	void RegisterCharacter(const FString& UID, AyyCharacter* Character);
	
	/** 注销角色 */
	void UnregisterCharacter(const FString& UID);
	
	/* 创建时character时获取stringID */
	FString GetCreateID();
	
	/** 获取所有角色 */
	UFUNCTION(BlueprintPure, Category = "yy|Manager")
	TArray<AyyCharacter*> GetAllCharacters();
	
private:
	/** UID -> 角色映射 */
	TMap<FString, TWeakObjectPtr<AyyCharacter>> CharacterMap;
	uint8 NextCharId = 0;
};
