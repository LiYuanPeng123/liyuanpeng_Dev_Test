#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "CancerInputSubsystem.generated.h"

class UInputAction;
class UInputMappingContext;

//输入层数据
USTRUCT(BlueprintType)
struct CANCERINPUT_API FInputLayerData
{
	GENERATED_USTRUCT_BODY()
	FInputLayerData()
	{
	};

	FInputLayerData(const FGameplayTag& InTag, bool bBlock = false):
		InputLayer(InTag), bBlockLowInputLayer(bBlock)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="输入层", meta=(Categories = "输入层"))
	FGameplayTag InputLayer;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="阻止低输入层")
	bool bBlockLowInputLayer = false;
};

//输入映射数据
USTRUCT(BlueprintType)
struct CANCERINPUT_API FInputActionMappingData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="输入映射")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="输入层")
	FGameplayTag InputLayer;
	//唯一Id
	FString Guid;
	FInputActionMappingData();
	FInputActionMappingData(UInputMappingContext* InputMappingContext, FGameplayTag InputLayer);
};

UCLASS()
class CANCERINPUT_API UCancerInputSubsystem : public UEnhancedInputLocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PlayerControllerChanged(APlayerController* NewPlayerController) override;

	UFUNCTION(BlueprintPure, Category = "Cancer Input|Manager Component")
	static UCancerInputSubsystem* GetCancerInputSubsystem(const AController* Controller);

	//输入层数据
	UPROPERTY()
	TMap<FString, FInputActionMappingData> InputActionDataMappingMap;
	//激活输入映射
	UFUNCTION(BlueprintCallable)
	FString ActivateInputMappingContext(FInputActionMappingData InputActionMappingData);
	//停止激活输入映射
	UFUNCTION(BlueprintCallable)
	void DeactivateInputMappingContext(FString InputGuid);
	//更新激活输入映射
	UFUNCTION()
	void UpdateInputMappingContext();
	
};
