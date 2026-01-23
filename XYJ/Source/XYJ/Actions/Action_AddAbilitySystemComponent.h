#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Actions/Action_AddActorComponent.h"
#include "Types/FCombatAbilitySet.h"
#include "Action_AddAbilitySystemComponent.generated.h"

class UStaggerAnimationDataAsset;
class UChooserTable;
class UGameplayAbility;
class UCancerCombatData;
class UCancerAbilitySet;
struct FCombatAbilitySet;
class UCancerAbilitySystemComponent;
class UCancerQuickBarComponent;
class UInventoryItemInstance;

UCLASS(DisplayName="技能组件")
class XYJ_API UAction_AddAbilitySystemComponent : public UAction_AddActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Ability, DisplayName="技能配置", meta=(
		RequiredAssetDataTags = "RowStructure=/Script/CancerAbility.CancerAbilityDataSet"))
	TSoftObjectPtr<UDataTable> AbilityData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Ability, DisplayName="特殊技配置", meta=(
			RequiredAssetDataTags = "RowStructure=/Script/CancerAbility.CancerComboData"))
	TSoftObjectPtr<UDataTable> SpecailAbilityData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Ability, DisplayName="受击配置")
	TSoftObjectPtr<UStaggerAnimationDataAsset> StaggerAnimationData;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= Ability,
		DisplayName="技能蒙太奇配置")
	TSoftObjectPtr<UChooserTable> ChooserTable;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= Ability,
    		DisplayName="属性配置")
	FComboAttributeSet ComboAttributeSet;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= Ability,
			DisplayName="升级GE")
	TSubclassOf<UGameplayEffect> LevelGE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Ability, DisplayName="战斗参数配置")
	TSoftObjectPtr<UCancerCombatData> CombatData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Ability, DisplayName="初始技能配置")
	TSoftObjectPtr<UCancerAbilitySet> StartAbilitySet;

	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;

protected:
	virtual void PostInitComponent_Implementation() override;

	virtual void FinalizeAfterComponent_Implementation() override;
};
