// Description:switch the language displayed in the editor
// Author:Jiecool
// Date:2024/5/8
// Email:jiecool@qq.com

#include "LanguageSwitchStyle.h"
#include "LanguageSwitch.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FLanguageSwitchStyle::StyleInstance = nullptr;

void FLanguageSwitchStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FLanguageSwitchStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FLanguageSwitchStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("LanguageSwitchStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FLanguageSwitchStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("LanguageSwitchStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("LanguageSwitch")->GetBaseDir() / TEXT("Resources"));

	Style->Set("LanguageSwitch.PluginAction", new IMAGE_BRUSH_SVG(TEXT("LanSwitchIcon"), Icon20x20));
	return Style;
}

void FLanguageSwitchStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FLanguageSwitchStyle::Get()
{
	return *StyleInstance;
}
