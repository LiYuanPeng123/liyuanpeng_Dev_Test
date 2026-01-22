
#include "TeamConfigDataAsset.h"

#include "CancerDefaultTeamRelationAsset.h"
#include "CancerTeamSetting.h"

uint8 UTeamConfigDataAsset::GetTeamIndex(const FGameplayTag& TeamTag) const
{
	const UCancerTeamSetting* TeamSetting = GetDefault<UCancerTeamSetting>();
	if (UCancerDefaultTeamRelationAsset* DefaultTeamAsset =  TeamSetting->DefaultTeamData.LoadSynchronous())
	{
		for (auto KVP : DefaultTeamAsset->TeamIndexMap)
		{
			if (TeamTag.MatchesTag(KVP.Key))
				return KVP.Value;
		}
	}
	return 0;
}

bool UTeamConfigDataAsset::FindTagByIndex(uint8 Index, FGameplayTag& OutTag) const
{
	const UCancerTeamSetting* TeamSetting = GetDefault<UCancerTeamSetting>();
	if (UCancerDefaultTeamRelationAsset* DefaultTeamAsset =  TeamSetting->DefaultTeamData.LoadSynchronous())
	{
		for (auto KVP : DefaultTeamAsset->TeamIndexMap)
		{
			if (KVP.Value == Index)
			{
				OutTag = KVP.Key;
				return true;
			}
		}
	}
	return false;
}

UTeamConfigDataAsset::UTeamConfigDataAsset()
{

}
