#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "FChooserType.generated.h"


//通用的Chooser结构体
USTRUCT(BlueprintType)
struct FChooserType
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer Target;

	FChooserType()
	{
		Target = FGameplayTagContainer();
	};

	FChooserType(const FGameplayTagContainer& InTag): Target(InTag)
	{
	}
};
