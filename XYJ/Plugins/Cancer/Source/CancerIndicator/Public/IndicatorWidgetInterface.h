#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IndicatorWidgetInterface.generated.h"

class AActor;
class UIndicatorDescriptor;

UINTERFACE(MinimalAPI, BlueprintType)
class UIndicatorWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class IIndicatorWidgetInterface
{
	GENERATED_BODY()

public:
	// 绑定指示器：传入描述器以初始化控件
	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
	void BindIndicator(UIndicatorDescriptor* Indicator);

	// 解绑指示器：清理引用与状态
	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
	void UnbindIndicator(const UIndicatorDescriptor* Indicator);

	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
	void UpdateIndicatorVisible(UUserWidget* InWidget,UIndicatorDescriptor* Indicator);

	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
	void IndicatorInteraction(UUserWidget* InWidget,UIndicatorDescriptor* Indicator);
};
