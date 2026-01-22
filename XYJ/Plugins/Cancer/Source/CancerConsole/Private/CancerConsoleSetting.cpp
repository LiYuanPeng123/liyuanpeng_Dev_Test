

#include "CancerConsoleSetting.h"

UCancerConsoleSetting* UCancerConsoleSetting::Get()
{
	return GetMutableDefault<UCancerConsoleSetting>();
}

FName UCancerConsoleSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerConsoleSetting::GetSectionName() const
{
	return FName(TEXT("Cancer控制台"));
}
