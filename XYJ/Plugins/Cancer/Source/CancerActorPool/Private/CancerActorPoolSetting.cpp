

#include "CancerActorPoolSetting.h"

UCancerActorPoolSetting* UCancerActorPoolSetting::Get()
{
	return GetMutableDefault<UCancerActorPoolSetting>();
}

FName UCancerActorPoolSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerActorPoolSetting::GetSectionName() const
{
	return FName(TEXT("CancerActorPool"));
}
