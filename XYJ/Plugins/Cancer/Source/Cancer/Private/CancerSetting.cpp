

#include "CancerSetting.h"

UCancerSetting* UCancerSetting::Get()
{
	return GetMutableDefault<UCancerSetting>();
}

FName UCancerSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerSetting::GetSectionName() const
{
	return FName(TEXT("Cancer"));
}
