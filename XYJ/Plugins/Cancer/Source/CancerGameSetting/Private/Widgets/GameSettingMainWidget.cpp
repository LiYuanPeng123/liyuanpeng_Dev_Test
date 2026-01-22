#include "Widgets/GameSettingMainWidget.h"
#include "GameSetting.h"
#include "GameSettingCollection.h"
#include "GameSettingRegistry.h"
#include "Widgets/GameSettingListView.h"
#include "Components/Button.h"
#include "Engine/Engine.h"
#include "GameSettingValueScalar.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "GameSettingAction.h"
#include "GameSettingManager.h" 
#include "GameSettingValueDynamic.h"

UGameSettingMainWidget::UGameSettingMainWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCategory = EGameSettingCategory::Graphics;
	bHasUnsavedChanges = false;
}

void UGameSettingMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 绑定按钮事件
	if (Button_Save)
	{
		Button_Save->OnClicked.AddDynamic(this, &UGameSettingMainWidget::OnSaveButtonClicked);
	}

	if (Button_Reset)
	{
		Button_Reset->OnClicked.AddDynamic(this, &UGameSettingMainWidget::OnResetButtonClicked);
	}

	if (Button_Apply)
	{
		Button_Apply->OnClicked.AddDynamic(this, &UGameSettingMainWidget::OnApplyButtonClicked);
	}

	if (Button_Cancel)
	{
		Button_Cancel->OnClicked.AddDynamic(this, &UGameSettingMainWidget::OnCancelButtonClicked);
	}

	// 初始化设置
	InitializeSettings();
}

void UGameSettingMainWidget::NativeDestruct()
{
	// 清理所有设置项事件绑定
	UnbindAllSettingEvents();
	
	// 清理资源
	CategoryCollections.Empty();

	Super::NativeDestruct();
}

void UGameSettingMainWidget::SwitchToCategory(EGameSettingCategory Category, bool bForceRefresh)
{
	if (CurrentCategory == Category && !bForceRefresh)
	{
		return;
	}

	CurrentCategory = Category;
	
	// 更新设置列表显示
	UpdateSettingsList();
	
	// 触发分类切换事件
	OnCategorySwitched.Broadcast(Category);
}

void UGameSettingMainWidget::InitializeSettings()
{
	// 使用 Registry 初始化所有设置
	UGameSettingRegistry* Registry = UGameSettingRegistry::Get();
	Registry->InitializeGameSettings();

	// 从 Registry 获取设置集合并映射到分类
	CategoryCollections.Empty();

	if (UGameSettingCollection* GraphicsSettings = Registry->GetSettingCollection(TEXT("Graphics")))
	{
		CategoryCollections.Add(EGameSettingCategory::Graphics, GraphicsSettings);
	}

	if (UGameSettingCollection* AudioSettings = Registry->GetSettingCollection(TEXT("Audio")))
	{
		CategoryCollections.Add(EGameSettingCategory::Audio, AudioSettings);
	}

	if (UGameSettingCollection* ControlsSettings = Registry->GetSettingCollection(TEXT("Controls")))
	{
		CategoryCollections.Add(EGameSettingCategory::Controls, ControlsSettings);
	}

	if (UGameSettingCollection* GameplaySettings = Registry->GetSettingCollection(TEXT("Gameplay")))
	{
		CategoryCollections.Add(EGameSettingCategory::Gameplay, GameplaySettings);
	}

	if (UGameSettingCollection* AccessibilitySettings = Registry->GetSettingCollection(TEXT("Accessibility")))
	{
		CategoryCollections.Add(EGameSettingCategory::Accessibility, AccessibilitySettings);
	}

	if (UGameSettingCollection*AdvancedSettings = Registry->GetSettingCollection(TEXT("Advanced")))
	{
		CategoryCollections.Add(EGameSettingCategory::Advanced, AdvancedSettings);
	}

	// 切换到默认分类（强制刷新）
	SwitchToCategory(CurrentCategory, true);

	UE_LOG(LogTemp, Log, TEXT("GameSettingMainWidget 初始化完成"));
}


void UGameSettingMainWidget::UpdateSettingsList()
{
	if (!SettingListView)
	{
		return;
	}

	// 清理旧的事件绑定
	UnbindAllSettingEvents();

	// 获取当前分类的设置集合
	if (TObjectPtr<UGameSettingCollection>* FoundCollection = CategoryCollections.Find(CurrentCategory))
	{
		if (UGameSettingCollection* Collection = FoundCollection->Get())
		{
			SettingListView->SetSettingCollection(Collection);
			
			// 为所有设置项绑定变化事件
			BindSettingEvents(Collection);
		}
	}
}

// 添加新方法：绑定设置项事件
void UGameSettingMainWidget::BindSettingEvents(UGameSettingCollection* Collection)
{
	if (!Collection)
	{
		return;
	}

	// 为集合中的所有设置项绑定变化事件
	TArray<UGameSetting*> Settings = Collection->GetChildSettings();
	for (UGameSetting* Setting : Settings)
	{
		if (Setting)
		{
			// 先移除可能存在的绑定，避免重复绑定
			Setting->OnSettingChanged.RemoveAll(this);
			// 重新绑定设置变化事件到主界面的处理函数
			Setting->OnSettingChanged.AddUObject(this, &UGameSettingMainWidget::OnSettingChanged);
		}
	}
}

// 添加新方法：解绑所有设置项事件
void UGameSettingMainWidget::UnbindAllSettingEvents()
{
	// 清理所有分类中设置项的事件绑定
	for (const auto& CollectionPair : CategoryCollections)
	{
		if (UGameSettingCollection* Collection = CollectionPair.Value.Get())
		{
			TArray<UGameSetting*> Settings = Collection->GetChildSettings();
			for (UGameSetting* Setting : Settings)
			{
				if (Setting)
				{
					Setting->OnSettingChanged.RemoveAll(this);
				}
			}
		}
	}
}

// 修复 SaveAllSettings 方法中类似的问题：
void UGameSettingMainWidget::SaveAllSettings()
{
	// 保存所有分类的设置
	for (const auto& CollectionPair : CategoryCollections)
	{
		if (UGameSettingCollection* Collection = CollectionPair.Value.Get())
		{
			// 注册所有设置项到注册表
			for (UGameSetting* Setting : Collection->GetChildSettings())
			{
				if (Setting)
				{
					UGameSettingRegistry::Get()->RegisterSetting(Setting);
				}
			}
		}
	}

	// 保存到配置文件
	UGameSettingRegistry::Get()->SaveSettings();

	bHasUnsavedChanges = false;
	
	// 触发保存完成事件
	OnSettingsSaved.Broadcast();
}

// 修复其他类似的 TMap::Find 使用：
void UGameSettingMainWidget::ResetCurrentCategoryToDefaults()
{
	if (TObjectPtr<UGameSettingCollection>* FoundCollection = CategoryCollections.Find(CurrentCategory))
	{
		if (UGameSettingCollection* Collection = FoundCollection->Get())
		{
			// 重置当前分类的所有设置
			for (UGameSetting* Setting : Collection->GetChildSettings())
			{
				if (Setting)
				{
					if (UGameSettingValue* ValueSetting = Cast<UGameSettingValue>(Setting))
					{
						ValueSetting->RestoreToInitial();
					}
				}
			}
		}
	}

	bHasUnsavedChanges = true;
	UpdateSettingsList();
}

void UGameSettingMainWidget::ResetAllSettingsToDefaults()
{
	// 重置所有分类的设置
	for (const auto& CollectionPair : CategoryCollections)
	{
		if (UGameSettingCollection* Collection = CollectionPair.Value.Get())
		{
			for (UGameSetting* Setting : Collection->GetChildSettings())
			{
				if (Setting)
				{
					if (UGameSettingValue* ValueSetting = Cast<UGameSettingValue>(Setting))
					{
						ValueSetting->RestoreToInitial();
					}
				}
			}
		}
	}

	bHasUnsavedChanges = true;
	UpdateSettingsList();
	
	// 触发重置完成事件
	OnSettingsReset.Broadcast();
}

void UGameSettingMainWidget::ApplySettings()
{
	// 应用所有设置更改
	for (const auto& CollectionPair : CategoryCollections)
	{
		if (UGameSettingCollection* Collection = CollectionPair.Value)
		{
			for (UGameSetting* Setting : Collection->GetChildSettings())
			{
				if (Setting)
				{
					Setting->OnApply();
				}
			}
		}
	}

	bHasUnsavedChanges = false;
}

void UGameSettingMainWidget::CancelChanges()
{
	// 取消所有未保存的更改
	for (const auto& CollectionPair : CategoryCollections)
	{
		if (UGameSettingCollection* Collection = CollectionPair.Value)
		{
			for (UGameSetting* Setting : Collection->GetChildSettings())
			{
				if (Setting)
				{
					Setting->RestoreToInitial();
				}
			}
		}
	}

	bHasUnsavedChanges = false;
	UpdateSettingsList();
}

bool UGameSettingMainWidget::HasUnsavedChanges() const
{
	return bHasUnsavedChanges;
}

void UGameSettingMainWidget::OnCategoryButtonClicked(EGameSettingCategory Category)
{
	SwitchToCategory(Category);
}

void UGameSettingMainWidget::OnSaveButtonClicked()
{
	SaveAllSettings();
}

void UGameSettingMainWidget::OnResetButtonClicked()
{
	ResetCurrentCategoryToDefaults();
}

void UGameSettingMainWidget::OnApplyButtonClicked()
{
	ApplySettings();
}

void UGameSettingMainWidget::OnCancelButtonClicked()
{
	CancelChanges();
}

void UGameSettingMainWidget::OnSettingChanged(UGameSetting* Setting, EGameSettingChangeReason Reason)
{
	bHasUnsavedChanges = true;
	
	// 如果启用自动保存，立即保存设置
	if (bAutoSave && Reason == EGameSettingChangeReason::Change)
	{
		SaveAllSettings();
	}
	
	// 触发设置更改事件
	OnSettingChangedEvent.Broadcast(Setting, bHasUnsavedChanges);
}