#include "Yy/Manager/YyCharacterManager.h"
#include "Yy/Character/yyCharacter.h"

UYyCharacterManager::UYyCharacterManager()
{
}

void UYyCharacterManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CharacterMap.Empty();
}

void UYyCharacterManager::Deinitialize()
{
	CharacterMap.Empty();
	Super::Deinitialize();
}

void UYyCharacterManager::RegisterCharacter(const FString& UID, AyyCharacter* Character)
{
	if (!CharacterMap.IsEmpty() && CharacterMap.Contains(UID)) 
		CharacterMap.Remove(UID);
	if (!UID.IsEmpty() && Character)
	{
		CharacterMap.Add(UID, Character);
	}
}

void UYyCharacterManager::UnregisterCharacter(const FString& UID)
{
	if (!UID.IsEmpty())
	{
		CharacterMap.Remove(UID);
	}
}

FString UYyCharacterManager::GetCreateID()
{
	return FString::Printf(TEXT("Character_%d"), NextCharId++);
}

TArray<AyyCharacter*> UYyCharacterManager::GetAllCharacters()
{
	TArray<AyyCharacter*> AllChar;
	for (TPair<FString, TWeakObjectPtr<AyyCharacter>> Map : CharacterMap)
	{
		if (Map.Value.IsValid())
		{
			AllChar.Add(Map.Value.Get());
		}
	}
	return AllChar;
}
