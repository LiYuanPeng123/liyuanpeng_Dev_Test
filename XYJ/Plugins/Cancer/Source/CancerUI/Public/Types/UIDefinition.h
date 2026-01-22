
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "UIDefinition.generated.h"

class UCancerUserWidget;
//输入设备类型
UENUM(BlueprintType)
enum class EInputDeviceType:uint8
{
	EDY_NONE UMETA(DisplayName = "未知"),
	EDY_MOUSE UMETA(DisplayName = "鼠标"),
	EDY_KEYBOARD UMETA(DisplayName = "键盘"),
	EDY_GAMEPAD_PS UMETA(DisplayName = "手柄PS"),
	EDY_GAMEPAD_XBOX UMETA(DisplayName = "手柄XBOX"),
};

//平台信息Key
USTRUCT(BlueprintType)
struct FPlatformInfoKey : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	//平台信息
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EInputDeviceType, FSlateBrush> PlatformBrushMap;
	//描述文本
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="描述", MultiLine="true"))
	FText Text;

	FPlatformInfoKey()
	{
		PlatformBrushMap.Empty();
		const UEnum* TagEnum = StaticEnum<EInputDeviceType>();
		for (int32 i = 0; i < TagEnum->NumEnums() - 1; i++)
		{
			PlatformBrushMap.Add(static_cast<EInputDeviceType>(i), FSlateBrush());
		}
	}
};

//UI信息
USTRUCT(BlueprintType)
struct FUIInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="UITag", Categories="UI"))
	FGameplayTag UITag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="UI类"))
	TSoftClassPtr<class UCancerUserWidget> UIClass;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="UI层", Categories="UI.Layer"))
	FGameplayTag UILayer;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="打开时隐藏图层", Categories="UI.Layer"))
	TArray<FGameplayTag> HideUILayer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="关闭时时显示图层", Categories="UI.Layer"))
	TArray<FGameplayTag> ShowUILayer;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="打开时隐藏其他UI", Categories="UI.UITag"))
	TArray<FGameplayTag> HideOtherUI;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="关闭时打开其他UI", Categories="UI.UITag"))
	TArray<FGameplayTag> ShowOtherUI;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="显示鼠标"))
	bool ShowMouseCursor = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="暂停游戏"))
	bool PauseGame = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="输入Mapping"))
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName="输入层", Categories="输入层"))
	FGameplayTag InputLayerTag;
	

	UPROPERTY()
	UCancerUserWidget* UICache = nullptr; //UI缓存

	bool ISValid() const 
	{
		return UITag.IsValid() && UIClass.ToSoftObjectPath().IsValid();
	};
};

//输入方向类型
UENUM(BlueprintType)
enum class EDirectionType : uint8
{
	DT_None UMETA(DisplayName = "没有输入"),
	DT_Up UMETA(DisplayName = "上"),
	DT_Down UMETA(DisplayName = "下"),
	DT_Left UMETA(DisplayName = "左"),
	DT_Right UMETA(DisplayName = "右"),
};

//Button状态
UENUM(BlueprintType)
enum class EButtonType : uint8
{
	Clicked UMETA(DisplayName = "点击"),
	Pressed UMETA(DisplayName = "按下"),
	Released UMETA(DisplayName = "松开"),
	Hovered UMETA(DisplayName = "悬停"),
	UnHovered UMETA(DisplayName = "未悬停"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputEventDelegate);

// ReSharper disable once UnrealHeaderToolError
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInputPointEventDelegate, const FPointerEvent&, PointerEvent,
											 const TSet<FKey>&, Keys);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputKeyEventDelegate, FKeyEvent, KeyEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputDeviceTypeChangeDelegate, EInputDeviceType, NewInputDeviceType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUIDisplayChangeDelegate, FGameplayTag, UIName);


