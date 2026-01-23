
#include "Action_AddTeamComponent.h"

#include "TeamConfigDataAsset.h"
#include "Components/CancerTeamComponent.h"

void UAction_AddTeamComponent::PostInitComponent_Implementation()
{
	UCancerTeamComponent* TeamComponent = Cast<UCancerTeamComponent>(GetCreateComponent());
	check(TeamComponent);
	TeamComponent->TeamTag = Team;
	
	// 同步控制器 GenericTeamId（用于感知与兼容逻辑）
	if (APawn* OwnerPawn = Cast<APawn>(GetCreateComponent()->GetOwner()))
	{
		if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OwnerPawn))
		{
			const uint8 Index = TeamComponent->GetTeamIndex(Team);
			TeamAgent->SetGenericTeamId(FGenericTeamId(Index));
		}
	}
	
}
