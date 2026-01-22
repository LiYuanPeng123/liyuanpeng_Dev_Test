

#include "CancerLockTargetSetting.h"

UCancerLockTargetSetting::UCancerLockTargetSetting()
{
}

UCancerLockTargetSetting* UCancerLockTargetSetting::Get()
{
	return GetMutableDefault<UCancerLockTargetSetting>();
}

FName UCancerLockTargetSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerLockTargetSetting::GetSectionName() const
{
	return FName(TEXT("CancerLockTarget"));
}
