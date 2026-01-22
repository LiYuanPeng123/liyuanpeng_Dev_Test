
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CancerCustomMode.generated.h"

UENUM(BlueprintType)
enum class ECancerCustomMovementMode : uint8
{
	MOVE_None		UMETA(DisplayName="None"),
	
	MOVE_WalkRun	UMETA(DisplayName="WalkRun"),
	
	MOVE_MAX		UMETA(Hidden),
};

/**
 * Cancer CustomMode
 */
UCLASS()
class CANCERANIMATION_API UCancerCustomMode : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, Category = "CustomMovementMode")
	bool IsActivate = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "CustomMovementMode")
	ECancerCustomMovementMode CustomMode{ECancerCustomMovementMode::MOVE_None};

	//更新 检测进去条件
	UFUNCTION(BlueprintNativeEvent, Category = "CustomMovementMode")
	bool TryEnterCustomMode();
	
	//进入前检测
	UFUNCTION(BlueprintNativeEvent, Category = "CustomMovementMode")
	bool CanEnterCustomMode();

	//开始
	UFUNCTION(BlueprintNativeEvent, Category = "CustomMovementMode")
	void EnterCustomMode();

	//更新
	UFUNCTION(BlueprintNativeEvent, Category = "CustomMovementMode")
	void UpdateCustomMode();

	//退出
	UFUNCTION(BlueprintNativeEvent, Category = "CustomMovementMode")
	void ExitCustomMode();

	//物理更新
	UFUNCTION(BlueprintNativeEvent, Category = "CustomMovementMode")
	void PhysCustom();

	UFUNCTION(BlueprintNativeEvent, Category = "CustomMovementMode")
	int32 GetCustomMode();
	
	UFUNCTION(BlueprintNativeEvent, Category = "CustomMovementMode")
	FString GetCustomModeDisplayName();
};
