#include "Common/CancerUIManagerSubsystem.h"
#include "CancerAssetManager.h"
#include "CancerInputSubsystem.h"
#include "CancerUISetting.h"
#include "CancerUserWidget.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagsManager.h"
#include "InputMappingContext.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Common/CancerCommonUILog.h"
#include "Common/CancerUIPolicy.h"
#include "Common/CancerLayout.h"
#include "Common/CancerWidgetStack.h"
#include "Components/CanvasPanelSlot.h"
#include "Input/CanerInputProcessor.h"
#include "Kismet/GameplayStatics.h"


class FSubsystemCollectionBase;
class UClass;

UE_DISABLE_OPTIMIZATION


namespace UIManger
{
	FName GetSimpleTag(FGameplayTag Tag)
	{
		const auto TagNode{UGameplayTagsManager::Get().FindTagNode(Tag)};
		return TagNode.IsValid() ? TagNode->GetSimpleTagName() : NAME_None;
	}
}

UCancerUIManagerSubsystem::UCancerUIManagerSubsystem()
{
}

void UCancerUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//是否启用CancerUI
	if (!UCancerUISetting::Get()->bEnabled)
	{
		return;
	}

	//编辑器模式检查 - 这里会提前返回
	if (Cast<UWorld>(GetOuter())->WorldType == EWorldType::Editor) return;

	if (TSubclassOf<UCancerUIPolicy> PolicyClass = UCancerUISetting::Get()->DefaultUIPolicyClass.LoadSynchronous())
		SwitchToPolicy(NewObject<UCancerUIPolicy>(this, PolicyClass));

	//注册输入预处理器
	if (!InputProcessor.IsValid())
	{
		InputProcessor = MakeShareable(new UCanerInputProcessor());
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor);
		}
		InputProcessor->InputDeviceTypeChangeDelegate.AddUniqueDynamic(
			this, &UCancerUIManagerSubsystem::InputDeviceTypeChange);
	}
	//使用重写后的空导航配置
	FSlateApplication::Get().SetNavigationConfig(MakeShared<FCancerNavigationConfig>());

	//TickHandle = FTSTicker::GetCoreTicker().AddTicker(
	//	FTickerDelegate::CreateUObject(this, &UCancerUIManagerSubsystem::Tick), 0.0);

	BeginPlay();
}

void UCancerUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	SwitchToPolicy(nullptr);

	if (InputProcessor.IsValid())
	{
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
		}
		InputProcessor.Reset();
	}

	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
}

void UCancerUIManagerSubsystem::OnWorldBeginTeardown(UWorld* InWorld)
{
}


void UCancerUIManagerSubsystem::InitInput()
{
	if (IsInitInput) return;
	IsInitInput = true;
	UWorld* CurWorld = GetWorld();
	if (!CurWorld) return;
	APlayerController* PlayerController = CurWorld->GetFirstPlayerController();
	if (!PlayerController) return;
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
	if (!EnhancedInputComponent) return;

	//绑定输入
	TArray<const UInputAction*> InputActionArray;
	for (auto Map : UIMap)
	{
		if (Map.Value.InputMappingContext)
		{
			for (FEnhancedActionKeyMapping Mapping : Map.Value.InputMappingContext->GetMappings())
			{
				const UInputAction* InputAction = Mapping.Action.Get();
				if (!InputActionArray.Contains(InputAction))
				{
					InputActionArray.Add(InputAction);
				}
			}
		}
	}
	for (const UInputAction* Action : InputActionArray)
	{
		EnhancedInputComponent->BindAction(Action, ETriggerEvent::Started, this, &ThisClass::ReceiveActionEvent,
		                                   Action, ETriggerEvent::Started);
		EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, this, &ThisClass::ReceiveActionEvent,
		                                   Action, ETriggerEvent::Triggered);
		EnhancedInputComponent->BindAction(Action, ETriggerEvent::Ongoing, this, &ThisClass::ReceiveActionEvent,
		                                   Action, ETriggerEvent::Ongoing);
		EnhancedInputComponent->BindAction(Action, ETriggerEvent::Canceled, this, &ThisClass::ReceiveActionEvent,
		                                   Action, ETriggerEvent::Canceled);
		EnhancedInputComponent->BindAction(Action, ETriggerEvent::Completed, this, &ThisClass::ReceiveActionEvent,
		                                   Action, ETriggerEvent::Completed);
	}
}

void UCancerUIManagerSubsystem::SetInputBlocking(bool NewInputBlock)
{
	InputBlock = NewInputBlock;
	UpdateShowMouseCursor();
}

void UCancerUIManagerSubsystem::ReceiveActionEvent(const UInputAction* InputAction, ETriggerEvent TriggerEvent)
{
	if (auto UIInfo = UIMap.Find(InputUIName))
	{
		UIInfo->UICache->ReceiveActionEvent(InputAction, TriggerEvent);
	}
}

void UCancerUIManagerSubsystem::SwitchToPolicy(UCancerUIPolicy* InPolicy)
{
	if (InPolicy)
	{
		CurrentPolicy = InPolicy;
		UIMap = CurrentPolicy->GetUIInfoMap();
	}
}

UCancerUserWidget* UCancerUIManagerSubsystem::GetUI(FGameplayTag UIName, TSubclassOf<UCancerUserWidget> Class)
{
	UCancerUserWidget* Widget = nullptr;

	if (Class)
	{
		for (auto Pair : UIMap)
		{
			if (Pair.Value.UIClass.Get() == Class)
			{
				return Pair.Value.UICache;
			}
		}
	}
	if (const auto UIInfo = UIMap.Find(UIName))
	{
		if (UIInfo->UICache)
		{
			Widget = UIInfo->UICache;
		}
	}
	return Widget;
}

UCancerUserWidget* UCancerUIManagerSubsystem::OpenUI(FGameplayTag UIName)
{
	if (!CurrentPolicy || !CurrentPolicy->GetRootLayout(this))
		return nullptr;
	TSubclassOf<UCancerUserWidget> UIClass = nullptr;
	if (const auto UIInfo = UIMap.Find(UIName))
	{
		//能否打开
		if (!OPenCheck(UIName, UIInfo))
		{
			return nullptr;
		}

		UIClass = UCancerAssetManager::GetSubclass(UIInfo->UIClass);

		if (UIClass)
		{
			UCancerUserWidget* Widget = nullptr;

			if (!UIInfo->UICache)
			{
				Widget = CreateWidget<UCancerUserWidget>(GetWorld(), UIClass);
				if (CurrentPolicy->GetRootLayout(this))
				{
					CurrentPolicy->AddWidget(UIInfo->UILayer, Widget);
				}
				if (!Widget)
				{
					return nullptr;
				}
				UIInfo->UICache = Widget;
			}
			else
			{
				Widget = UIInfo->UICache;
				UIInfo->UICache->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}

			//对应Layer 添加打开的UI
			FGameplayTagContainer UITags;
			if (UIShowMap.Contains(UIInfo->UILayer))
			{
				FGameplayTagContainer CacheTags = UIShowMap.FindOrAdd(UIInfo->UILayer);
				UITags.AppendTags(CacheTags);
			}
			UITags.AddTag(UIName);
			UIShowMap.Add(UIInfo->UILayer, UITags);

			//UI打开时调用
			Widget->OnOpened();

			//添加禁止标签
			for (auto Tag : UIInfo->HideUILayer)
			{
				HideUILayer.AddTag(Tag);
			}
			for (auto Tag : UIInfo->HideOtherUI)
			{
				HideOtherUI.AddTag(Tag);
			}

			//更新Layer和Tag
			UpdateLayerAndTag();

			//UI排序
			SortAllUI();

			//更新鼠标
			UpdateShowMouseCursor();

			//暂停游戏
			UpdatePauseGame();

			return Widget;
		}
	}
	return nullptr;
}

bool UCancerUIManagerSubsystem::OPenCheck(const FGameplayTag& UIName, const FUIInfo* UIInfo)
{
	//是否属于禁止打开的Layer
	if (HideUILayer.HasTagExact(UIInfo->UILayer))
	{
		UE_LOG(CancerCommonUILog, Warning, TEXT("禁止打开Layer:%s UIName:%s"),
		       *UIManger::GetSimpleTag(UIInfo->UILayer).ToString(),
		       *UIManger::GetSimpleTag(UIName).ToString())
		return false;
	}
	//是否禁止打开的UItag
	if (HideOtherUI.HasTagExact(UIName))
	{
		UE_LOG(CancerCommonUILog, Warning, TEXT("禁止打开UIName:%s"),
		       *UIManger::GetSimpleTag(UIName).ToString())
		return false;
	}
	//是否已经打开
	if (UIShowMap.FindOrAdd(UIInfo->UILayer).HasTagExact(UIName))
	{
		return false;
	}
	return true;
}

void UCancerUIManagerSubsystem::CloseUI(FGameplayTag UIName, bool bDestroy, UCancerUserWidget* InWidget)
{
	if (!CurrentPolicy || !CurrentPolicy->GetRootLayout(this))
		return;
	UCancerUserWidget* CloseWidget = nullptr;
	FUIInfo UIInfo;
	if (InWidget)
	{
		for (auto Pair : UIMap)
		{
			if (Pair.Value.UICache == InWidget)
			{
				UIInfo = Pair.Value;
				UIName = Pair.Key;
				CloseWidget = InWidget;
			}
		}
	}
	else
	{
		CloseWidget = GetUI(UIName);
		UIInfo = GetUInfo(CloseWidget);
	}
	if (!CloseWidget)
	{
		return;
	}
	//UIClose 调用
	CloseWidget->OnClosed();

	if (bDestroy)
	{
		UIMap[UIName].UICache = nullptr;
		CurrentPolicy->RemoveWidget(CloseWidget);
		CloseWidget->RemoveFromParent();
		CloseWidget->ConditionalBeginDestroy();
	}
	else
	{
		//隐藏
		CloseWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	FGameplayTagContainer UINames;
	if (UIShowMap.Contains(UIInfo.UILayer))
	{
		FGameplayTagContainer CacheUINames = UIShowMap.FindOrAdd(UIInfo.UILayer);
		UINames.AppendTags(CacheUINames);
	}
	UINames.RemoveTag(UIName);
	UIShowMap.Add(UIInfo.UILayer, UINames);


	//删除禁止标签
	for (auto Tag : UIInfo.HideUILayer)
	{
		HideUILayer.RemoveTag(Tag);
		if (RemoveHideLayerDelegate.IsBound())
		{
			RemoveHideLayerDelegate.Broadcast(Tag);
		}
	}
	for (auto Tag : UIInfo.HideOtherUI)
	{
		HideOtherUI.RemoveTag(Tag);
		if (RemoveHideLayerDelegate.IsBound())
		{
			RemoveHideLayerDelegate.Broadcast(Tag);
		}
	}
	for (auto Tag : UIInfo.ShowUILayer)
	{
		ShowUILayer.AddTag(Tag);
	}
	for (auto Tag : UIInfo.ShowOtherUI)
	{
		ShowOtherUI.AddTag(Tag);
	}

	UpdateLayerAndTag();

	//UI排序
	SortAllUI();

	//更新鼠标
	UpdateShowMouseCursor();

	//暂停游戏
	UpdatePauseGame();
}

void UCancerUIManagerSubsystem::ClearUILayout(FGameplayTag LayoutName)
{
	if (!CurrentPolicy->GetRootLayout(this))
		return;

	if (UIShowMap.Contains(LayoutName))
	{
		TArray<FGameplayTag> UITArray = UIShowMap.FindOrAdd(LayoutName).GetGameplayTagArray();
		for (auto Tag : UITArray)
		{
			CloseUI(Tag, true);
		}
	}
}

FUIInfo UCancerUIManagerSubsystem::GetUInfo(UCancerUserWidget* InWidget)
{
	for (auto Pair : UIMap)
	{
		if (Pair.Value.UICache == InWidget)
		{
			return Pair.Value;
		}
	}
	return FUIInfo();
}

FUIInfo UCancerUIManagerSubsystem::GetUInfo(FGameplayTag& Intag)
{
	for (auto Pair : UIMap)
	{
		if (Pair.Key.MatchesTagExact(Intag))
		{
			return Pair.Value;
		}
	}
	return FUIInfo();
}

void UCancerUIManagerSubsystem::UpdatePauseGame()
{
	bool PauseGame = false;
	for (auto UI : UIMap)
	{
		if (UI.Value.PauseGame)
		{
			if (UI.Value.UICache && UI.Value.UICache->IsVisible())
			{
				PauseGame = true;
				break;
			}
		}
	}
	UGameplayStatics::SetGamePaused(GetWorld(), PauseGame);
}

void UCancerUIManagerSubsystem::UpdateLayerAndTag()
{
	UpdateHideOtherUI();
	UpdateHideOtherLayer();
	UpdateShowOtherUI();
	UpdateShowOtherLayer();
}

void UCancerUIManagerSubsystem::UpdateHideOtherUI()
{
	//从显示UI中移除
	for (auto UITag : HideOtherUI.GetGameplayTagArray())
	{
		FUIInfo UIInfo = UIMap.FindOrAdd(UITag);
		if (!UIInfo.ISValid())
		{
			continue;
		}

		if (auto HideUI = UIMap.FindOrAdd(UITag).UICache)
		{
			FUIInfo UIInfo1 = UIMap.FindOrAdd(UITag);
			if (!UIInfo1.ISValid())
			{
				continue;
			}
			//是否已经关闭
			if (!UIShowMap.FindOrAdd(UIInfo1.UILayer).HasTagExact(UITag))
			{
				continue;
			}
			CloseUI(UITag, false);
		}
	}
}

void UCancerUIManagerSubsystem::UpdateHideOtherLayer()
{
	for (auto LayerTag : HideUILayer.GetGameplayTagArray())
	{
		CurrentPolicy->GetRootLayout(this)->
		               GetLayerWidget(LayerTag)->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCancerUIManagerSubsystem::UpdateShowOtherUI()
{
	for (auto UITag : ShowOtherUI.GetGameplayTagArray())
	{
		FUIInfo UIInfo = GetUInfo(UITag);
		if (!UIInfo.ISValid())
		{
			continue;
		}
		//是否属于禁止打开的Layer
		if (HideUILayer.HasTagExact(UIInfo.UILayer))
		{
			continue;
		}
		//是否禁止打开的UItag
		if (HideOtherUI.HasTagExact(UITag))
		{
			continue;
		}
		//是否已经打开
		if (UIShowMap.FindOrAdd(UIInfo.UILayer).HasTagExact(UITag))
		{
			continue;
		}
		OpenUI(UITag);
	}
	ShowOtherUI.Reset();
}

void UCancerUIManagerSubsystem::UpdateShowOtherLayer()
{
	for (auto LayerTag : ShowUILayer.GetGameplayTagArray())
	{
		CurrentPolicy->GetRootLayout(this)->
		               GetLayerWidget(LayerTag)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	ShowUILayer.Reset();
}

void UCancerUIManagerSubsystem::UpdateShowMouseCursor()
{
	//是否有UI还需要鼠标
	bool ShowMouseCursor = false;
	switch (GetInputDeviceType())
	{
	case EInputDeviceType::EDY_NONE: break;
	case EInputDeviceType::EDY_MOUSE:
		{
			for (auto UI : UIMap)
			{
				if (UI.Value.ShowMouseCursor)
				{
					if (const UCancerUserWidget* Widget = UI.Value.UICache)
					{
						if (Widget->IsVisible())
						{
							ShowMouseCursor = true;
							break;
						}
					}
					if (ShowMouseCursor)
					{
						break;
					}
				}
			}
		}
		break;
	case EInputDeviceType::EDY_KEYBOARD:
		for (auto UI : UIMap)
		{
			if (UI.Value.ShowMouseCursor)
			{
				if (const UCancerUserWidget* Widget = UI.Value.UICache)
				{
					if (Widget->IsVisible())
					{
						ShowMouseCursor = true;
						break;
					}
				}
				if (ShowMouseCursor)
				{
					break;
				}
			}
		}
		break;
	case EInputDeviceType::EDY_GAMEPAD_PS:
	case EInputDeviceType::EDY_GAMEPAD_XBOX:
		break;
	default: ;
	}


#if PLATFORM_ANDROID || PLATFORM_IOS
	ShowMouseCursor = true;
#endif
	//设置模式

	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (ShowMouseCursor)
			{
				//聚焦当前界面
				if (const auto UIInfo = UIMap.Find(InputUIName))
				{
					//UCommonUIExtensions::FocusTargetWidget(Widget);
				}

				//只能设置一次,因为会吞掉鼠标事件
				if (!bInputModeGameAndUI)
				{
					UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController);
					bInputModeGameAndUI = true;
				}
			}
			else
			{
				//GameOnly可以反复设置,因为鼠标不被捕获
				UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
				bInputModeGameAndUI = false;
			}
			PlayerController->SetShowMouseCursor(ShowMouseCursor);
		}
	}
}

void UCancerUIManagerSubsystem::TopUINameSort(FGameplayTag UIName, bool IsAdd)
{
}

void UCancerUIManagerSubsystem::SortAllUI()
{
	//排序
	int ZOrderIndex = 0;
	FGameplayTag LastInputName = InputUIName;
	InputUIName = FGameplayTag();
	const TArray<FGameplayTag>& Array = GetDefault<UCancerUISetting>()->LayerSort;
	for (int i = 0; i < Array.Num(); ++i)
	{
		FGameplayTag UI_Sort = Array[i];
		if (!UIShowMap.Contains(UI_Sort)) continue;
		TArray<FGameplayTag> UINameArray = UIShowMap[UI_Sort].GetGameplayTagArray();
		for (FGameplayTag UIName : UINameArray)
		{
			const auto UIInfo = UIMap.Find(UIName);
			if (const auto UI = UIInfo->UICache)
			{
				if (UIInfo->InputMappingContext)
				{
					InputUIName = UIName;
				}
				if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(UI->Slot))
				{
					CanvasPanelSlot->SetZOrder(ZOrderIndex);
					ZOrderIndex++;
				}
				UI->bEnableInput = false;
			}
		}
	}

	if (auto LastUI = GetUI(InputUIName))
	{
		LastUI->bEnableInput = true;
		UE_LOG(CancerCommonUILog, Warning, TEXT("输入启用: %s"), *InputUIName.ToString());
	}

	//输入映射
	if (UCancerInputSubsystem* CancerInputSubsystem = ULocalPlayer::GetSubsystem<UCancerInputSubsystem>(
		CurrentPolicy->GetRootLayout(this)->GetOwningLocalPlayer()))
	{
		InitInput();
		if (!InputUIName.MatchesTagExact(LastInputName))
		{
			if (LastInputName.IsValid())
			{
				CancerInputSubsystem->DeactivateInputMappingContext(InputGuid);
				InputGuid.Empty();
			}
			if (InputUIName.IsValid())
			{
				if (FUIInfo* UIInfo = UIMap.Find(InputUIName))
				{
					if (UIInfo->InputMappingContext)
					{
						if (!UIInfo->InputLayerTag.IsValid())
						{
							//没有配置 采用默认设置的输入层
							UIInfo->InputLayerTag = UCancerUISetting::Get()->UIInputTag;
						}
						InputGuid = CancerInputSubsystem->ActivateInputMappingContext(
							FInputActionMappingData(UIInfo->InputMappingContext, UIInfo->InputLayerTag));
					}
				}
			}
		}
	}
}

bool UCancerUIManagerSubsystem::Tick(float DeltaTime)
{
	SyncRootLayoutVisibilityToShowHUD();
	return true;
}

void UCancerUIManagerSubsystem::SyncRootLayoutVisibilityToShowHUD()
{
	/*if (UCancerUIPolicy* Policy = GetCurrentUIPolicy())
	{
		bool bShouldShowUI = true;

		if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			const AHUD* HUD = PC->GetHUD();

			if (HUD && !HUD->bShowHUD)
			{
				bShouldShowUI = false;
			}
		}

		if (UCancerLayout* RootLayout = Policy->GetRootLayout(this))
		{
			const ESlateVisibility DesiredVisibility = bShouldShowUI
				                                           ? ESlateVisibility::SelfHitTestInvisible
				                                           : ESlateVisibility::Collapsed;
			if (DesiredVisibility != RootLayout->GetVisibility())
			{
				RootLayout->SetVisibility(DesiredVisibility);
			}
		}
	}*/
}

EInputDeviceType UCancerUIManagerSubsystem::GetInputDeviceType() const
{
	if (const auto InputProcessorObj = InputProcessor.Get())
	{
		return InputProcessorObj->InputDeviceType;
	}
	return EInputDeviceType::EDY_KEYBOARD;
}

void UCancerUIManagerSubsystem::InputDeviceTypeChange(EInputDeviceType NewInputDeviceType)
{
	UpdateShowMouseCursor();
	InputDeviceTypeChangeDelegate.Broadcast(NewInputDeviceType);
}

UE_ENABLE_OPTIMIZATION
