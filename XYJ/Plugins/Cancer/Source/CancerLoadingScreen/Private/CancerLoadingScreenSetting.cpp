

#include "CancerLoadingScreenSetting.h"

UCancerLoadingScreenSetting::UCancerLoadingScreenSetting()
{

}

UCancerLoadingScreenSetting* UCancerLoadingScreenSetting::Get()
{
	return GetMutableDefault<UCancerLoadingScreenSetting>();
}

FName UCancerLoadingScreenSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerLoadingScreenSetting::GetSectionName() const
{
	return FName(TEXT("CancerLoadingScreen"));
}
