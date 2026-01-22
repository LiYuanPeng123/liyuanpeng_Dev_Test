#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ComboInputMapping.generated.h"

USTRUCT(BlueprintType)
struct FComboChord
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FGameplayTag> Inputs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag Output;
};

UCLASS(BlueprintType)
class CANCERABILITY_API UComboInputMapping : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, FGameplayTag> PhysicalToLogical;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FComboChord> Chords;

	UFUNCTION(BlueprintCallable)
	FGameplayTag MapPhysicalToLogical(const FGameplayTag& Physical) const;

	UFUNCTION(BlueprintCallable)
	FGameplayTag TryResolveChord(const TArray<FGameplayTag>& PressedInWindow) const;
};

