


#include "CancerUISetting.h"

#include "Common/CancerUIPolicy.h"

UCancerUISetting::UCancerUISetting()
{
	DefaultUIPolicyClass = UCancerUIPolicy::StaticClass();
}

UCancerUISetting* UCancerUISetting::Get()
{
	return GetMutableDefault<UCancerUISetting>();
}

FName UCancerUISetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerUISetting::GetSectionName() const
{
	return FName(TEXT("CancerUI"));
}
