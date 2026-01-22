#pragma once

#include "CoreMinimal.h"
#include "CancerAbilityInputConfig.h"
#include "CancerInputConfig.generated.h"

class UInputHandler_InputAbility;
enum class ETriggerEvent : uint8;
class UCancerInputHandler;
class UInputAction;
class UInputMappingContext;

UCLASS()
class CANCERINPUT_API UCancerInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UCancerInputConfig();

	void InitInput() const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "Inputcontext"))
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "Inputcontext"))
	float Priority;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Handler")
	bool bCanBeBuffered;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Handler", meta = (EditCondition = "bCanBeBuffered"))
	FGameplayTag BufferChannelTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer|Input", DisplayName="输入数据"
		, meta=(RequiredAssetDataTags = "RowStructure=/Script/CancerInput.CancerInputAction"))
	TObjectPtr<UDataTable> InputDataTable;

	TArray<FCancerInputAction> GetInputInfo() const
	{
		TArray<FCancerInputAction> InfoMap;

		if (!InputDataTable) return InfoMap;
		TArray<FCancerInputAction*> InfoArray;
		InputDataTable->GetAllRows(FString(), InfoArray);
		for (const auto& Info : InfoArray)
		{
			InfoMap.Add(*Info);
		}
		return InfoMap;
	}

	UPROPERTY(BlueprintReadOnly, Category = "Input Setup")
	TObjectPtr<UInputHandler_InputAbility> InputHandler;

	bool HasCompatibleInputHandler(const UInputAction* InputAction, const ETriggerEvent& TriggerEvent) const;

#if WITH_EDITOR

	// Editor functionalities.
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) override;

#endif
};
