
#pragma once

#include "CoreMinimal.h"
#include "CancerInputSubsystem.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "CancerInputSettings.generated.h"

enum class ETriggerEvent : uint8;


UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Cancer Input"))
class CANCERINPUT_API UCancerInputSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UCancerInputSettings();

	/**
    * Enables data validation for the Setup Asset.
    * 
    * Highly recommended to keep this enabled to avoid common mistakes in the Setup Data!
    */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Development")
	bool bEnableDataValidation;

	/**
    * If enabled, it will ensure that all Input Actions from the Context have an equivalent Handler.
    *
    * Can only be used if the Data Validation is enabled.
    */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Development",
		meta = (EditCondition = bEnableDataValidation, EditConditionHides))
	bool bMatchHandlersWithContext;

	/** Gameplay Tag that, if present, blocks camera movement. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Input Blocking")
	FGameplayTag BlockCameraTag;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Input Blocking")
	FGameplayTag BlockMovementTag;

	/** Gameplay Tag that, if present, blocks character rotation. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Input Blocking")
	FGameplayTag BlockRotationTag;

	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Core Settings")
	TArray<ETriggerEvent> TrackedEvents;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Development")
	bool bShowScreenDebugMessages;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Development",
		meta = (EditCondition = bShowScreenDebugMessages, EditConditionHides))
	float DebugMessageDuration;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Development",
		meta = (EditCondition = bShowScreenDebugMessages, EditConditionHides))
	FColor DebugMessageColor;

	UPROPERTY(Config, EditAnywhere, meta = (DisplayName="默认启用输入层"))
	FGameplayTag DefaultGameInputTag;

	UPROPERTY(Config, EditAnywhere, meta = (DisplayName="输入层数据"))
	TArray<FInputLayerData> InputLayerDataArray;


	virtual FName GetCategoryName() const override;

	virtual FName GetSectionName() const override;

	static UCancerInputSettings* Get();
};
