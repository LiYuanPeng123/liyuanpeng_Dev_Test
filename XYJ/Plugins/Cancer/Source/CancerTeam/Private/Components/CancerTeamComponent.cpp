#include "Components/CancerTeamComponent.h"

#include "CancerTeamAgentInterface.h"
#include "CancerTeamSetting.h"
#include "TeamConfigDataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

UCancerTeamComponent::UCancerTeamComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCancerTeamComponent::BeginPlay()
{
	Super::BeginPlay();
}

static FGameplayTag NormalizeToFirstTwoSegments(const FGameplayTag& Tag)
{
    const FString TagString = Tag.ToString();
    TArray<FString> Parts;
    TagString.ParseIntoArray(Parts, TEXT("."), true);
    if (Parts.Num() >= 2)
    {
        const FString Prefix = Parts[0] + TEXT(".") + Parts[1];
        return UGameplayTagsManager::Get().RequestGameplayTag(FName(*Prefix), /*ErrorIfNotFound*/false);
    }
    return Tag;
}

ETeamAttitude::Type UCancerTeamComponent::GetAttitude(FGameplayTag SelfTeam, FGameplayTag TargetTeam) const
{
    if (!TeamTag.IsValid())
    {
        return ETeamAttitude::Neutral;
    }

	// 自身的特殊阵营关系
    const FGameplayTag NormTargetTeam = NormalizeToFirstTwoSegments(TargetTeam);
	const FGameplayTag NormSelfTeam = NormalizeToFirstTwoSegments(SelfTeam);

	// 项目的通用队伍关系
    if (const UCancerTeamSetting* TeamSetting = GetDefault<UCancerTeamSetting>())
    {
	    if (UCancerDefaultTeamRelationAsset* TeamAsset =  TeamSetting->DefaultTeamData.LoadSynchronous())
	    {
	    	
	    	// AB阵营只需要配置一个 不论是通过自身或通过对方都可以找到相对关系
	    	if (FTeamInfo* TeamInfo = TeamAsset->TeamInfo.Find(NormSelfTeam))
	    	{
	    		if (TEnumAsByte<ETeamAttitude::Type>* Attitude =  TeamInfo->Relationship.Find(NormTargetTeam))
	    		{
	    			return Attitude->GetValue();
	    		}
	    	}
	    	
	    	if (FTeamInfo* TeamInfo = TeamAsset->TeamInfo.Find(NormTargetTeam))
	    	{
	    		if (TEnumAsByte<ETeamAttitude::Type>* Attitude =  TeamInfo->Relationship.Find(NormSelfTeam))
	    		{
	    			return Attitude->GetValue();
	    		}
	    	}
	    }
    }

    return ETeamAttitude::Neutral;
}

FGenericTeamId UCancerTeamComponent::GetGenericTeamId() const
{
	return FGenericTeamId(GetTeamIndex(TeamTag));
}

bool UCancerTeamComponent::IsEnemyTeam(FGameplayTag SelfTeam, FGameplayTag TargetTeam, ETeamAttitude::Type TeamAttitude) const
{
	return GetAttitude(SelfTeam, TargetTeam) == TeamAttitude;
}

uint8 UCancerTeamComponent::GetTeamIndex(const FGameplayTag& InTeamTag) const
{
	const UCancerTeamSetting* TeamSetting = GetDefault<UCancerTeamSetting>();
	if (UCancerDefaultTeamRelationAsset* DefaultTeamAsset =  TeamSetting->DefaultTeamData.LoadSynchronous())
	{
		for (auto KVP : DefaultTeamAsset->TeamIndexMap)
		{
			if (InTeamTag.MatchesTag(KVP.Key))
				return KVP.Value;
		}
	}
	return 0;
}

bool UCancerTeamComponent::FindTagByIndex(uint8 Index, FGameplayTag& OutTag) const
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

void UCancerTeamComponent::SetTeamTag(FGameplayTag NewTeamTag)
{
	if (TeamTag != NewTeamTag)
	{
		const FGenericTeamId OldId = GetGenericTeamId();
		TeamTag = NewTeamTag;
		const FGenericTeamId NewId = GetGenericTeamId();

		if (OldId != NewId)
		{
			if (AActor* Owner = GetOwner())
			{
				//更改控制器中SetGenericTeamId
				if (APawn* PawnOwner = Cast<APawn>(Owner))
				{
					if (AController* Controller = PawnOwner->GetController())
					{
						if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Controller))
						{
							TeamAgent->SetGenericTeamId(NewId);
						}
					}
				}
				if (Owner->Implements<UCancerTeamAgentInterface>())
				{
					ICancerTeamAgentInterface::ConditionalBroadcastTeamChanged(Owner, OldId, NewId);
				}
			}
		}
	}
}