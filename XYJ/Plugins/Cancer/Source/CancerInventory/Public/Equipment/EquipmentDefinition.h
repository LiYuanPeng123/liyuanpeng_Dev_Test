#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "EquipmentDefinition.generated.h"

class UCancerAbilitySet;
class UEquipmentInstance;


UENUM(BlueprintType)
enum class EEquipSocket:uint8
{
	Equip = 0 UMETA(DisplayName="收刀状态"),
	Combo = 1 UMETA(DisplayName="拔刀状态"),
	Default = 255 UMETA(Hidden),
};


USTRUCT()
struct FEquipmentActorToSpawn
{
	GENERATED_BODY()

	FEquipmentActorToSpawn()
	{
	}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	TMap<EEquipSocket,FName> AttachSockets;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
};


USTRUCT(BlueprintType)
struct FEquipmentInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Equipment)
	AActor* SpawnActor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Equipment)
	TMap<EEquipSocket,FName> AttachSockets;
};



/**
 * 装备定义
 */
UCLASS(Blueprintable, Abstract, BlueprintType, DisplayName="装备定义")
class CANCERINVENTORY_API UEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category=Equipment, DisplayName="装备实例")
	TSubclassOf<UEquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category=Equipment, DisplayName="装备赋予的能力")
	TArray<TObjectPtr<UCancerAbilitySet>> AbilitySetsToGrant;

	UPROPERTY(EditDefaultsOnly, Category=Equipment, DisplayName="装备生成的Actor")
	TArray<FEquipmentActorToSpawn> ActorsToSpawn;
};
