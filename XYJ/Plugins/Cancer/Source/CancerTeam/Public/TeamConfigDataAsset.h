
#pragma once

#include "CoreMinimal.h"
#include "TeamCoreType.h"
#include "Engine/DataAsset.h"
#include "TeamConfigDataAsset.generated.h"

UCLASS()
class CANCERTEAM_API UTeamConfigDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

	public:
	UTeamConfigDataAsset();

	UFUNCTION(BlueprintPure, Category = Cancer)
	uint8 GetTeamIndex(const FGameplayTag& TeamTag) const;

	UFUNCTION(BlueprintPure, Category = Cancer)
	bool FindTagByIndex(uint8 Index, FGameplayTag& OutTag) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cancer, meta=(Categories="阵营"))
	FGameplayTag OwnerTeam;
};
