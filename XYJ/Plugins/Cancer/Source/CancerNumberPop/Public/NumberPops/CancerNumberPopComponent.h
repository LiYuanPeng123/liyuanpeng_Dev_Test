#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CancerPawnComponent.h"

#include "CancerNumberPopComponent.generated.h"

USTRUCT(BlueprintType)
struct FCancerNumberPopRequest
{
	GENERATED_BODY()

	// The world location to create the number pop at
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer|Number Pops")
	FVector WorldLocation;

	// Tags related to the source/cause of the number pop (for determining a style)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer|Number Pops")
	FGameplayTagContainer SourceTags;

	// Tags related to the target of the number pop (for determining a style)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cancer|Number Pops")
	FGameplayTagContainer TargetTags;

	// The number to display
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer|Number Pops")
	int32 NumberToDisplay = 0;

	// Whether the number is 'critical' or not (@TODO: move to a tag)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer|Number Pops")
	bool bIsCriticalDamage = false;

	FCancerNumberPopRequest()
		: WorldLocation(ForceInitToZero)
	{
	}
};


UCLASS(Abstract)
class CANCERNUMBERPOP_API UCancerNumberPopComponent : public UCancerPawnComponent
{
	GENERATED_BODY()

public:
	UCancerNumberPopComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Adds a damage number to the damage number list for visualization */
	UFUNCTION(BlueprintCallable, Category = Foo)
	virtual void AddNumberPop(const FCancerNumberPopRequest& NewRequest)
	{
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="显示伤害")
	bool bIsShowDamage = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName="显示治疗")
	bool bIsShowCue = false;
};
