

#include "CancerBaseAIController.h"


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
