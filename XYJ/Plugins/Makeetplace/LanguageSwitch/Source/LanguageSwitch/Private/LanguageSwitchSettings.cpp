// Description:switch the language displayed in the editor
// Author:Jiecool
// Date:2024/5/8
// Email:jiecool@qq.com

#include "LanguageSwitchSettings.h"

ULanguageSwitchSettings::ULanguageSwitchSettings()
{

}

FName ULanguageSwitchSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

FText ULanguageSwitchSettings::GetSectionText() const
{
	return NSLOCTEXT("LanguageSwitchSettings", "LanguageSwitchSectionText", "Language Switch");
}

FName ULanguageSwitchSettings::GetSectionName() const
{
	return TEXT("Language Switch");
}