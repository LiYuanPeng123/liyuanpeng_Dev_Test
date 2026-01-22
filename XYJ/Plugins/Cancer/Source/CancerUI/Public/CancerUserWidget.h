
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/UIDefinition.h"
#include "CancerUserWidget.generated.h"


enum class ETriggerEvent : uint8;
class UInputAction;

UENUM(BlueprintType)
enum class ECancerWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

UCLASS(Abstract, Blueprintable)
class CANCERUI_API UCancerUserWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloseDlegate);
	
	UPROPERTY(BlueprintAssignable)
	FOnCloseDlegate OnClose;
	
	//每次Open_UI的时候会走的流程
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnOpened();
	
	//每次Close_UI会走的流程
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnClosed();

	bool bEnableInput = false;
	//输入映射
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ReceiveActionEvent(const UInputAction* InputAction, ETriggerEvent TriggerEvent);

	//接受数据
	UFUNCTION(BlueprintNativeEvent)
	void HandleReceviedData(UObject* Object);
	
	//输入设备变更
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ReceiveInputDeviceChange(EInputDeviceType NewInputDeviceType);
};
