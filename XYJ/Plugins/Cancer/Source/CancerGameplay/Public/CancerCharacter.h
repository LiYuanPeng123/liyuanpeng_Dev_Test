#pragma once

#include "CoreMinimal.h"
#include "CancerTeamAgentInterface.h"
#include "GameplayTagContainer.h"
#include "TeamConfigDataAsset.h"
#include "Components/CancerTeamComponent.h"
#include "GameFramework/Character.h"
#include "CancerCharacter.generated.h"

UCLASS()
class CANCERGAMEPLAY_API ACancerCharacter : public ACharacter, public ICancerTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACancerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement|Tags")
	TMap<TEnumAsByte<EMovementMode>, FGameplayTag> MovementModeTagMap;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement|Tags")
	TMap<uint8, FGameplayTag> CustomMovementModeTagMap;
	
#pragma region 团队接口
	virtual FOnCancerTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override
	{
		return &OnCancerTeamIndexChangedDelegate;
	};
	UPROPERTY(BlueprintAssignable)
	FOnCancerTeamIndexChangedDelegate OnCancerTeamIndexChangedDelegate;

	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override
	{
		if (UCancerTeamComponent* TeamComp = FindComponentByClass<UCancerTeamComponent>())
		{
			if (TeamComp->GetTeamsConfiguration())
			{
				FGameplayTag Tag;
				if (TeamComp->GetTeamsConfiguration()->FindTagByIndex(TeamID.GetId(), Tag))
				{
					TeamComp->TeamTag = Tag;
				}
			}
		}
	}

	virtual FGenericTeamId GetGenericTeamId() const override
	{
		if (const UCancerTeamComponent* TeamComp = FindComponentByClass<UCancerTeamComponent>())
		{
			if (TeamComp->GetTeamsConfiguration())
			{
				return FGenericTeamId(TeamComp->GetTeamsConfiguration()->GetTeamIndex(TeamComp->TeamTag));
			}
		}
		return FGenericTeamId(0);
	}

#pragma endregion
};
