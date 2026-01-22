
#include "CancerInventorySetting.h"


UCancerInventorySetting* UCancerInventorySetting::Get()
{
	return GetMutableDefault<UCancerInventorySetting>();
}

FName UCancerInventorySetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerInventorySetting::GetSectionName() const
{
	return FName(TEXT("CancerInventory"));
}
