

#include "CancerNumberPopSetting.h"

UCancerNumberPopSetting* UCancerNumberPopSetting::Get()
{
	return GetMutableDefault<UCancerNumberPopSetting>();
}

FName UCancerNumberPopSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerNumberPopSetting::GetSectionName() const
{
	return FName(TEXT("CancerNumberPop"));
}
