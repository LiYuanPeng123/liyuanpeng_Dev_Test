
#include "CancerActionCreatorSetting.h"

UCancerActionCreatorSetting* UCancerActionCreatorSetting::Get()
{
	return GetMutableDefault<UCancerActionCreatorSetting>();
}

FName UCancerActionCreatorSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerActionCreatorSetting::GetSectionName() const
{
	return FName(TEXT("CancerActionCreator"));
}
