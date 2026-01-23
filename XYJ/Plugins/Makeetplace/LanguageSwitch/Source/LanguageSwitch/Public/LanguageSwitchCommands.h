// Description:switch the language displayed in the editor
// Author:Jiecool
// Date:2024/5/8
// Email:jiecool@qq.com

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "LanguageSwitchStyle.h"

class FLanguageSwitchCommands : public TCommands<FLanguageSwitchCommands>
{
public:

	FLanguageSwitchCommands()
		: TCommands<FLanguageSwitchCommands>(TEXT("LanguageSwitch"), NSLOCTEXT("Contexts", "LanguageSwitch", "LanguageSwitch Plugin"), NAME_None, FLanguageSwitchStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
