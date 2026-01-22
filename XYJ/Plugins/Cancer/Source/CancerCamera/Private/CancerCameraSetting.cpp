
#include "CancerCameraSetting.h"

UCancerCameraSetting* UCancerCameraSetting::Get()
{
	return GetMutableDefault<UCancerCameraSetting>();
}

FName UCancerCameraSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}
