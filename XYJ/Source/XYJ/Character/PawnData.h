#pragma once
#include "CoreMinimal.h"
#include "Action_DataAssetTemplate.h"
#include "Engine/DataAsset.h"
#include "PawnData.generated.h"

class UTeamConfigDataAsset;
class UStaggerAnimationDataAsset;
class UCancerCombatData;
class UCancerAbilityTagSetMapping;
class AXYJ_Character;

/**
 * 
 */
UCLASS()
class XYJ_API UPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Pawn, DisplayName="Action")
	TSoftObjectPtr<UAction_DataAsset> ActionData;

};
