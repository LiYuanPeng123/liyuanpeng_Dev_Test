#pragma once

#include "CoreMinimal.h"
#include "CancerCoreFunctionLibrary.h"
#include "Subsystems/WorldSubsystem.h"
#include "CancerActorSubsystem.generated.h"


class UAction_DataAsset;
//ActorList
USTRUCT()
struct FCancerActorList
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<AActor*> Actors;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorCreated, const AActor*, Actor);
/**
 * 项目中生成的各种Actor
 */
UCLASS()
class CANCERACTIONCREATOR_API UCancerActorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnActorCreated  OnActorCreated;
	
	UFUNCTION()
	void OnActorDistory(AActor* Actor, EEndPlayReason::Type EndPlayReason);

	//注册Actor
	UFUNCTION(BlueprintCallable, Category = "Cancer|Actor")
	void RegisterActor(const UAction_DataAsset* ActorData, AActor* NewActor);

	// 获取单个Actor (返回第一个找到的)
	UFUNCTION(BlueprintCallable, Category = "Cancer|Actor")
	AActor* GetActor(const FGameplayTag& ActorTag);

	// 获取指定Tag的所有Actor
	UFUNCTION(BlueprintCallable, Category = "Cancer|Actor")
	TArray<AActor*> GetActors(const FGameplayTag& ActorTag);

	// 按照Tag寻找Actor
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> FindCharacterByQuery(const TArray<FCancerQueryMatch>& TagQuery);

private:
	UPROPERTY()
	TMap<FGameplayTag, FCancerActorList> ActorMap;
};
