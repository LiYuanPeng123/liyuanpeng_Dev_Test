

#include "CancerIndicatorSetting.h"

UCancerIndicatorSetting* UCancerIndicatorSetting::Get()
{
	return GetMutableDefault<UCancerIndicatorSetting>();
}

FName UCancerIndicatorSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerIndicatorSetting::GetSectionName() const
{
	return FName(TEXT("CancerIndicator"));
}
