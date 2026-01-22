#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "ICancerPickupable.generated.h"


class UCancerInteractionDefinition;
class UCancerInventoryComponent;
class UInventoryItemInstance;
class UInventoryItemDefinition;

USTRUCT(BlueprintType)
struct FPickupTemplate
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta=(Categories="道具"))
	FGameplayTag ItemTag;

	FPickupTemplate(){};
	FPickupTemplate(const FGameplayTag& InItemTag,int32 InCount)
	{
		ItemTag = InItemTag;
		StackCount = InCount;
	}
	
};

USTRUCT(BlueprintType)
struct FPickupInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInventoryItemInstance> Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackCount = 1;
};

USTRUCT(BlueprintType)
struct FInventoryPickup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPickupInstance> Instances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPickupTemplate> Templates;
};


UINTERFACE(BlueprintType)
class UCancerPickupable : public UInterface
{
	GENERATED_BODY()
};

class CANCERINVENTORY_API ICancerPickupable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FGameplayTag GetPickupItemTag();
	virtual FGameplayTag GetPickupItemTag_Implementation() { return FGameplayTag(); };

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetPickupItemTag(const FGameplayTag& PickupItemTag);
	
	UFUNCTION(BlueprintNativeEvent)
	void SetInteractionDef(UCancerInteractionDefinition* InteractionDef);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandleInteraction(FGameplayEventData EventData);

	virtual void HandleInteraction_Implementation(FGameplayEventData EventData)
	{
	};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractionStart(FGameplayEventData EventData);

	virtual void InteractionStart_Implementation(FGameplayEventData EventData)
	{
	};
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractionFinish(FGameplayEventData EventData);
	virtual void InteractionFinish_Implementation(FGameplayEventData EventData)
	{
	};
};

UCLASS()
class UPickupableStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UPickupableStatics();

public:
	UFUNCTION(BlueprintPure)
	static TScriptInterface<ICancerPickupable> GetFirstPickupableFromActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	static void AddPickupToInventory(UCancerInventoryComponent* InventoryComponent,
	                                 TScriptInterface<ICancerPickupable> Pickup);

	UFUNCTION(BlueprintPure, Category="Inventory")
	static FInventoryPickup MakeInventoryPickup(const TArray<FPickupInstance>& Instances,
	                                            const TArray<FPickupTemplate>& Templates);
};
