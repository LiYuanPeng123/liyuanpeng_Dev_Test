
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CancerInteractionActor.h"
#include "Interfaces/CancerWeaponInterface.h"
#include "CancerWeaponActor.generated.h"

UCLASS()
class CANCERABILITY_API ACancerWeaponActor : public ACancerInteractionActor, public ICancerWeaponInterface
{
	GENERATED_BODY()

public:
	ACancerWeaponActor()
	{
	};

	virtual UMeshComponent* GetInteractionMesh_Implementation() override;
};
