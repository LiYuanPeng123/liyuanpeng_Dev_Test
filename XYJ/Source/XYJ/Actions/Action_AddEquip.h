#pragma once

#include "CoreMinimal.h"
#include "ActionDef.h"
#include "GameplayTagContainer.h"
#include "Actions/CancerCreatorActionBase.h"
#include "Data/CancerAbilitySet.h"
#include "Equipment/EquipmentDefinition.h"
#include "Action_AddEquip.generated.h"

class UCancerQuickBarInstance;



UCLASS(DisplayName="添加道具")
class XYJ_API UAction_AddEquip : public UCancerCreatorActionBase
{
	GENERATED_BODY()

public:
	UAction_AddEquip();

	template <typename T>
	static TArray<T> GetData(UDataTable* Data)
	{
		TArray<T> OutTableStruct;
		if (!Data) return OutTableStruct;
		TArray<T*> RowPtrs;
		Data->GetAllRows<T>(FString(), RowPtrs);
		for (auto* RowPtr : RowPtrs)
		{
			OutTableStruct.Add(*RowPtr);
		}
		return OutTableStruct;
	}


	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const override;
	virtual void PostInitComponent_Implementation() override;
	virtual void FinalizeAfterComponent_Implementation() override;

protected:
	UPROPERTY(Instanced,  BlueprintReadOnly)
	TArray<UCancerQuickBarInstance*> QuickBarInstances;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grant")
	ECancerGrantSource GrantItemSource;

	UPROPERTY(EditDefaultsOnly, Category="Grant", meta=(RequiredAssetDataTags = "RowStructure=/Script/XYJ.ItemSet",
		EditCondition="GrantItemSource == ECancerGrantSource::DataTables", EditConditionHides))
	TSoftObjectPtr<UDataTable> ItemSetData;

	UPROPERTY(EditDefaultsOnly, Category="Grant",
		meta=(EditCondition="GrantItemSource == ECancerGrantSource::Arrays", EditConditionHides))
	TArray<FItemSet> ActivateItemSets;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grant")
	ECancerGrantSource GrantEquipSource;

	UPROPERTY(EditDefaultsOnly, Category="Grant",meta=(RequiredAssetDataTags = "RowStructure=/Script/XYJ.EquipSet",
		EditCondition="GrantEquipSource == ECancerGrantSource::DataTables", EditConditionHides
	))
	TSoftObjectPtr<UDataTable> EquipSetData;
	UPROPERTY(EditDefaultsOnly, Category="Grant",
		meta=(EditCondition="GrantEquipSource == ECancerGrantSource::Arrays", EditConditionHides))
	TArray<FEquipSet> ActivateEquipSets;

	UPROPERTY(EditDefaultsOnly, Category="Grant")
	EEquipSocket EquipSocket = EEquipSocket::Equip;

	UPROPERTY(EditDefaultsOnly, Category="Grant")
	int32 SlotID = 0;
};
