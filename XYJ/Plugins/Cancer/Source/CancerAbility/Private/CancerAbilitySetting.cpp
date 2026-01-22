

#include "CancerAbilitySetting.h"

UCancerAbilitySetting* UCancerAbilitySetting::Get()
{
	return GetMutableDefault<UCancerAbilitySetting>();
}

FName UCancerAbilitySetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerAbilitySetting::GetSectionName() const
{
	return FName(TEXT("Cancer技能"));
}
