#pragma once

#include "CoreMinimal.h"
#include "Components/CancerTeamComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "CancerBaseAIController.generated.h"

class IEntityInterface;

UCLASS()
class CANCERAI_API ACancerBaseAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* PossPawn) override;

	virtual void OnUnPossess() override;

	virtual FGenericTeamId GetGenericTeamId() const override
	{
		if (UCancerTeamComponent* TeamComp = GetPawn()->FindComponentByClass<UCancerTeamComponent>())
		{
			return TeamComp->GetGenericTeamId();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ACancerBaseAIController -- GetGenericTeamId"))
		}
		return FGenericTeamId();
	};

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override
	{
		Super::SetGenericTeamId(InTeamID);
		PerceptionComponent->RequestStimuliListenerUpdate();
		//同步到玩家组件
		if (UCancerTeamComponent* TeamComp = GetPawn()->FindComponentByClass<UCancerTeamComponent>())
		{
			FGameplayTag Tag;
			if (TeamComp->FindTagByIndex(InTeamID.GetId(), Tag))
			{
				TeamComp->TeamTag = Tag;
			}
		}	else
		{
			UE_LOG(LogTemp,Error,TEXT("角色未实现--IGenericTeamAgentInterface--"))
		}
	};

	// IGenericTeamAgentInterface
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override
	{
		if (GetPawn()->Implements<UGenericTeamAgentInterface>())
		{
			IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn());
			return TeamAgent->GetTeamAttitudeTowards(Other);
		}
		UE_LOG(LogTemp,Error,TEXT("角色未实现--IGenericTeamAgentInterface--"))
		return ETeamAttitude::Friendly;
	};

public:
	ACancerBaseAIController();

};
