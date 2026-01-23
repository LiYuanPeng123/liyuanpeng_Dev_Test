

#pragma once

#include "CoreMinimal.h"
#include "CancerCoreFunctionLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "CancerCharacterSubsystem.generated.h"

/**
 * 查找项目中生成的各种角色
 */
UCLASS()
class CANCERGAMEPLAY_API UCancerCharacterSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void OnCharacterDead(AActor* Actor , EEndPlayReason::Type EndPlayReason);
	
	// 有新的角色生成
	void AddCharacter(AActor* InNewCharacter);
	
	// 按照Tag寻找角色
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> FindCharacterByQuery(const TArray<FCancerQueryMatch>& TagQuery);
	
	// 每次生成新的角色 都会添加在这里
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> SpawnedCharacters; 
};
