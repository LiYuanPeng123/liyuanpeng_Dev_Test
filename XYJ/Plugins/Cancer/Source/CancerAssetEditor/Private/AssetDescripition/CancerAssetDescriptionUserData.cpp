#include "AssetDescripition/CancerAssetDescriptionUserData.h"

void UCancerAssetDescriptionUserData::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	OutTags.Emplace(TEXT("Description"), Description, FAssetRegistryTag::TT_Alphabetical);
}
