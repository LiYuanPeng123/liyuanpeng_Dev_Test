#include "CancerArchiveSetting.h"

UCancerArchiveSetting* UCancerArchiveSetting::Get()
{
	return GetMutableDefault<UCancerArchiveSetting>();
}

FName UCancerArchiveSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerArchiveSetting::GetSectionName() const
{
	return FName(TEXT("Cancer存档"));
}
