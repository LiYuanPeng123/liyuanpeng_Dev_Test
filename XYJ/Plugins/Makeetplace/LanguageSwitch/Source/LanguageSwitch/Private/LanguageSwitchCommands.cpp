// Description:switch the language displayed in the editor
// Author:Jiecool
// Date:2024/5/8
// Email:jiecool@qq.com

#include "LanguageSwitchCommands.h"

#define LOCTEXT_NAMESPACE "FLanguageSwitchModule"

void FLanguageSwitchCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "LanguageSwitch", "Language Switch", EUserInterfaceActionType::Button, FInputChord(EKeys::L));
}

#undef LOCTEXT_NAMESPACE
