
#pragma once

#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Framework/Application/NavigationConfig.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/UIDefinition.h"
#include "UObject/SoftObjectPtr.h"
#include "CancerUIManagerSubsystem.generated.h"

class UInputAction;
class UCanerInputProcessor;
class FSubsystemCollectionBase;
class UCommonLocalPlayer;
class UCancerUIPolicy;
class UObject;

/**
 * This manager is intended to be replaced by whatever your game needs to
 * actually create, so this class is abstract to prevent it from being created.
 * 
 * If you just need the basic functionality you will start by sublcassing this
 * subsystem in your own game.
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateRemoveHideLayer, FGameplayTag,LayoutTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateRemoveHideOtherUI, FGameplayTag,UITag);

class FCancerNavigationConfig : public FNullNavigationConfig
{
public:
	//空导航配置类 (否则空格会导航被占用)
	virtual EUINavigationAction GetNavigationActionForKey(const FKey& InKey) const override
	{
		//鼠标触发聚焦时接收到的所有按键全部返回无效
		return EUINavigationAction::Invalid;
	}
};

UCLASS(BlueprintType, Blueprintable)
class CANCERUI_API UCancerUIManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UCancerUIManagerSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	
	void OnWorldBeginTeardown(UWorld* InWorld);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "GameUIManagerSubsystem")
	void BeginPlay();
	
	const UCancerUIPolicy* GetCurrentUIPolicy() const { return CurrentPolicy; }
	UCancerUIPolicy* GetCurrentUIPolicy() { return CurrentPolicy; }

	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FDelegateRemoveHideLayer RemoveHideLayerDelegate;
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FDelegateRemoveHideOtherUI FDelegateRemoveHideOtherUI;
	
	
	UCancerUserWidget* GetUI(FGameplayTag UIName,TSubclassOf<UCancerUserWidget> Class = nullptr);
	UFUNCTION(BLueprintCallable,Category="UIManagerSubsystem")
	UCancerUserWidget* OpenUI(FGameplayTag UIName);
	
	UFUNCTION(BLueprintCallable,Category="UIManagerSubsystem")
	void CloseUI(FGameplayTag UIName,bool bDestroy = false,UCancerUserWidget* InWidget = nullptr);
	
#pragma  region  //输入
	//输入预处理器
	TSharedPtr<UCanerInputProcessor> InputProcessor;

	//获取输入设备类型
	EInputDeviceType GetInputDeviceType() const;
	//输入设备变更委托函数
	UFUNCTION()
	void InputDeviceTypeChange(EInputDeviceType NewInputDeviceType);

	//输入设备更改委托
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FInputDeviceTypeChangeDelegate InputDeviceTypeChangeDelegate;
	
	//绑定输入
	UPROPERTY()
	bool IsInitInput;
	UFUNCTION(BlueprintCallable)
	void InitInput();
	//输入阻断
	UPROPERTY()
	bool InputBlock;
	//设置输入阻断
	UFUNCTION(BlueprintCallable)
	void SetInputBlocking(bool NewInputBlock);

	//输入函数
	void ReceiveActionEvent(const UInputAction* InputAction, ETriggerEvent TriggerEvent);

	//输入UI
	UPROPERTY()
	FGameplayTag InputUIName;
	//输入Guid
	UPROPERTY()
	FString InputGuid;
	//需要UIInputMapping
	UPROPERTY()
	bool bNeedUIInputMapping;
#pragma endregion
	
protected:
	void SwitchToPolicy(UCancerUIPolicy* InPolicy);
	
	bool OPenCheck(const FGameplayTag& UIName,const FUIInfo* UIInfo);
	
	void ClearUILayout(FGameplayTag LayoutName);
	
	FUIInfo GetUInfo(UCancerUserWidget* InWidget);

	FUIInfo GetUInfo(FGameplayTag& Intag);

	//更新暂停游戏
	void UpdatePauseGame();

	//更新显示隐藏Layer 和 Tag
	void UpdateLayerAndTag();
	void UpdateHideOtherUI();
	void UpdateHideOtherLayer();
	void UpdateShowOtherUI();
	void UpdateShowOtherLayer();

	//更新鼠标
	void UpdateShowMouseCursor();
	
	//将该UI置顶
	void TopUINameSort(FGameplayTag UIName, bool IsAdd);
	
	//排序UI层级，调整ZOrder
	void SortAllUI();

private:
	bool Tick(float DeltaTime);
	
	void SyncRootLayoutVisibilityToShowHUD();
	
	FTSTicker::FDelegateHandle TickHandle;
	
private:
	bool bInputModeGameAndUI = false;

	//需要隐藏的Layer 
	FGameplayTagContainer HideUILayer;
	//需要隐藏的UI 
	FGameplayTagContainer HideOtherUI;

	//需要显示的Layer 
	FGameplayTagContainer ShowUILayer;
	//需要显示的UI 
	FGameplayTagContainer ShowOtherUI;
	
	UPROPERTY(Transient)
	TObjectPtr<UCancerUIPolicy> CurrentPolicy = nullptr;

	UPROPERTY()
	TMap<FGameplayTag, FUIInfo> UIMap;

	//显示的UI
	TMap<FGameplayTag, FGameplayTagContainer> UIShowMap;
};
