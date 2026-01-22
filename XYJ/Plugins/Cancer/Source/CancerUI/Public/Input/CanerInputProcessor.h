

#pragma once




#include "CoreMinimal.h"
#include "Types/UIDefinition.h"
#include "Framework/Application/IInputProcessor.h"

class CANCERUI_API UCanerInputProcessor : public IInputProcessor, public TSharedFromThis<UCanerInputProcessor>
{
public:
	UCanerInputProcessor();
	
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent,const FPointerEvent* InGestureEvent) override;
	virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override;

	//更新设备类型
	void UpdateInputDeviceType(const FKeyEvent& InKeyEvent);
	void SetInputDeviceType(EInputDeviceType NewInputDeviceType);
	
	//设备类型
	EInputDeviceType InputDeviceType = EInputDeviceType::EDY_KEYBOARD;
	
	//输入设备更改委托
	FInputDeviceTypeChangeDelegate InputDeviceTypeChangeDelegate;
	
	//输入按键委托
	FInputKeyEventDelegate InputKeyDown;
	FInputKeyEventDelegate InputKeyUp;
	
	//按键委托
	FInputPointEventDelegate MouseDownDelegate;
	FInputPointEventDelegate MouseUpDelegate;
	FInputPointEventDelegate MouseDownMoveDelegate;
	FInputEventDelegate MouseMoveDelegate;

	//用于记录当前按下了哪些鼠标键
	TSet<FKey> PressedKeys;

	//鼠标移动阈值
	float MouseMoveThreshold = 1;
	//手柄移动阈值
	float GamepadAxisThreshold = 0.2;

	//上帧鼠标位置
	FVector2D LastMouseLocation;
};
