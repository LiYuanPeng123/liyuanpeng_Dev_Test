// Description:switch the language displayed in the editor
// Author:Jiecool
// Date:2024/5/8
// Email:jiecool@qq.com

#include "LanguageSwitch.h"
#include "LanguageSwitchStyle.h"
#include "LanguageSwitchCommands.h"
#include "LanguageSwitchSettings.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "Kismet/KismetInternationalizationLibrary.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Editor/InternationalizationSettings/Classes/InternationalizationSettingsModel.h"
#include "Runtime/Core/Public/Internationalization/Internationalization.h"

#include "Interfaces/IMainFrameModule.h"

static const FName LanguageSwitchTabName("LanguageSwitch");

#define LOCTEXT_NAMESPACE "FLanguageSwitchModule"

void FLanguageSwitchModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FLanguageSwitchStyle::Initialize();
	FLanguageSwitchStyle::ReloadTextures();

	FLanguageSwitchCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FLanguageSwitchCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FLanguageSwitchModule::PluginButtonClicked),
		FCanExecuteAction());

	IMainFrameModule& MainFrame = FModuleManager::GetModuleChecked<IMainFrameModule>("MainFrame");

	#if ENGINE_MAJOR_VERSION >=5
		TSharedPtr<FUICommandList> MainFrameCommands = MainFrame.GetMainFrameCommandBindings();
	#else
		TSharedRef<FUICommandList> MainFrameCommands = MainFrame.GetMainFrameCommands();
	#endif

	MainFrameCommands->Append(PluginCommands.ToSharedRef());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLanguageSwitchModule::RegisterMenus));
}

void FLanguageSwitchModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FLanguageSwitchStyle::Shutdown();

	FLanguageSwitchCommands::Unregister();
}

void FLanguageSwitchModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	if (GConfig) {
		//TArray<FCultureRef> LocalizedCulturesForEditor;
		//constexpr bool bIncludeDerivedCultures = false;
		const TArray<FString> LocalizedCultureNames = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Editor);
		//LocalizedCulturesForEditor = FInternationalization::Get().GetAvailableCultures(LocalizedCultureNames, bIncludeDerivedCultures);

		// 获取SwitchLocaleSettings的实例
		ULanguageSwitchSettings* LocaleSettings = GetMutableDefault<ULanguageSwitchSettings>();

		// 读取SourceLanguage属性的值
		ELangs SourceLang = LocaleSettings->SourceLanguage;
		ELangs TargetLang = LocaleSettings->TargetLanguage;

		if (LocaleSettings) {
			FString SourceLangStr;
			FString TargetLangStr;
			UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ELangs"), true);
			if (EnumPtr)
			{
				SourceLangStr = EnumPtr->GetNameStringByValue((uint8)SourceLang);
				TargetLangStr = EnumPtr->GetNameStringByValue((uint8)TargetLang);
				SourceLangStr.ReplaceInline(TEXT("_"), TEXT("-"));
				TargetLangStr.ReplaceInline(TEXT("_"), TEXT("-"));

				FString CurrentLanguage = UKismetInternationalizationLibrary::GetCurrentLanguage();
				FString NewLanguage = (CurrentLanguage == SourceLangStr) ? TargetLangStr : SourceLangStr;
				TWeakObjectPtr<UInternationalizationSettingsModel> SettingsModel;
				bool bExist = LocalizedCultureNames.Contains(NewLanguage);
				if (bExist) {
					SettingsModel->SetEditorLanguage(NewLanguage);
					/*if (not SettingsModel->ShouldUseLocalizedPropertyNames()) {
						SettingsModel->SetShouldUseLocalizedPropertyNames(bExist);
					}
					if (not SettingsModel->ShouldUseLocalizedNodeAndPinNames()) {
						SettingsModel->SetShouldUseLocalizedNodeAndPinNames(bExist);
					}*/
					FInternationalization& I18N = FInternationalization::Get();
					I18N.SetCurrentLanguage(NewLanguage);
					// Find all Schemas and force a visualization cache clear
					for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
					{
						UClass* CurrentClass = *ClassIt;

						if (UEdGraphSchema* Schema = Cast<UEdGraphSchema>(CurrentClass->GetDefaultObject()))
						{
							Schema->ForceVisualizationCacheClear();
						}
					}
				}
			}
		}
	}
}

void FLanguageSwitchModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("LanguageSwitch");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FLanguageSwitchCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
				Entry.Name = "LanguageSwitchButton";
				Entry.Label = FText::FromString(TEXT(""));
				Entry.ToolTip = FText::FromString(TEXT("LanguageSwitch"));
				Entry.ToolTip = LOCTEXT("LanguageSwitch_Tooltip", "Global Keyboard Shortcut:");
			}
		}
	}

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("AssetEditorToolbar.CommonActions");
	{
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("LanguageSwitch");
		{
			FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FLanguageSwitchCommands::Get().PluginAction));
			Entry.SetCommandList(PluginCommands);
			Entry.Name = "LanguageSwitchButton";
			Entry.Label = FText::FromString(TEXT(""));
			Entry.ToolTip = FText::FromString(TEXT("LanguageSwitch"));
			Entry.ToolTip = LOCTEXT("LanguageSwitch_Tooltip", "Global Keyboard Shortcut:");
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLanguageSwitchModule, LanguageSwitch)