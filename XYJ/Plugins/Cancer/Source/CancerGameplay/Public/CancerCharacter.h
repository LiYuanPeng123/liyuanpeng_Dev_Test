#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CancerTeamAgentInterface.h"
#include "GameplayTagContainer.h"
#include "TeamConfigDataAsset.h"
#include "Components/CancerTeamComponent.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "CancerCharacter.generated.h"

class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FOnCancerMovementModeChangedDelegate, class ACancerCharacter*, Character,
                                               EMovementMode, PrevMovementMode, uint8, PreviousCustomMode, FGameplayTag,
                                               PrevTag, EMovementMode, NewMovementMode, uint8, NewCustomMode,
                                               FGameplayTag, NewTag);

UCLASS()
class CANCERGAMEPLAY_API ACancerCharacter : public ACharacter, public ICancerTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACancerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);
	FGameplayTag GetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode) const;

	UPROPERTY(BlueprintAssignable)
	FOnCancerMovementModeChangedDelegate OnMovementModeChangedDelegate;

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
			FGameplayTag Tag;
			if (TeamComp->FindTagByIndex(TeamID.GetId(), Tag))
			{
				TeamComp->TeamTag = Tag;
			}
		}
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			//同步到控制器中的TeamId
			IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(AIController);
			TeamAgent->SetGenericTeamId(TeamID);
			AIController->PerceptionComponent->RequestStimuliListenerUpdate();
		}
	}

	virtual FGenericTeamId GetGenericTeamId() const override
	{
		if (const UCancerTeamComponent* TeamComp = FindComponentByClass<UCancerTeamComponent>())
		{
			return FGenericTeamId(TeamComp->GetTeamIndex(TeamComp->TeamTag));
		}
		return FGenericTeamId(0);
	}

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override
	{
		if (const UCancerTeamComponent* MyTeamComp = FindComponentByClass<UCancerTeamComponent>())
		{
			if (const UCancerTeamComponent* OtherTeamComp = Other.FindComponentByClass<UCancerTeamComponent>())
			{
				return MyTeamComp->GetAttitude(MyTeamComp->TeamTag, OtherTeamComp->TeamTag);
			}
		}
		return ETeamAttitude::Friendly;
	}
#pragma endregion
};
