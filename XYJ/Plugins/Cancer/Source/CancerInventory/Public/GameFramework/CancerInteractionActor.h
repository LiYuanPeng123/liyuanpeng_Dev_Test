
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ICancerInteraction.h"
#include "CancerInteractionActor.generated.h"

UCLASS()
class CANCERINVENTORY_API ACancerInteractionActor : public AActor,public IICancerInteraction
{
	GENERATED_BODY()

public:
	ACancerInteractionActor();
};
