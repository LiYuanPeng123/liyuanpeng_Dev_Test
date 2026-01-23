

#include "CancerBaseAIController.h"

#include "Components/CancerTeamComponent.h"
#include "Perception/AIPerceptionComponent.h"


ACancerBaseAIController::ACancerBaseAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACancerBaseAIController::OnPossess(APawn* PossPawn)
{
	Super::OnPossess(PossPawn);
}

void ACancerBaseAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

