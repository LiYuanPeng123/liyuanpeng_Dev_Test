#pragma once
#include "CoreMinimal.h"
#include "CancerInputSettings.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "FInputTriggerData.generated.h"


enum class ETriggerEvent : uint8;
class UInputAction;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInputTrigger, FGameplayEventData, EventData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputTriggerMC, FGameplayEventData, EventData);


USTRUCT(BlueprintType)
struct FInputTriggerHandle
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGuid Id;
};


USTRUCT(BlueprintType)
struct FInputTriggerData
{
	GENERATED_BODY()

public:
	bool operator==(const FInputTriggerData& Other) const
	{
		return TriggerAction == Other.TriggerAction && TriggerEvent == Other.TriggerEvent;
	}

	void RemoveBindEvent()
	{
		OnTriggerEvent.RemoveAll(Source.Get());
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> TriggerAction{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETriggerEvent TriggerEvent{ETriggerEvent::Triggered};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = -1;
	FOnInputTriggerMC OnTriggerEvent;
	UPROPERTY()
	TWeakObjectPtr<UObject> Source;
};
