#pragma once

#include "CoreMinimal.h"
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

public:
	ACancerBaseAIController();

};
