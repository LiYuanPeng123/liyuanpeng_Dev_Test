

#include "CancerCoreSetting.h"

UCancerCoreSetting* UCancerCoreSetting::Get()
{
	return GetMutableDefault<UCancerCoreSetting>();
}

FName UCancerCoreSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerCoreSetting::GetSectionName() const
{
	return FName(TEXT("CancerCore"));
}
