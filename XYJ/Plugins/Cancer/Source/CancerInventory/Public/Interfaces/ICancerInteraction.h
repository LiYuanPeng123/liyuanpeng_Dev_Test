#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICancerInteraction.generated.h"

UINTERFACE()
class UICancerInteraction : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CANCERINVENTORY_API IICancerInteraction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	UMeshComponent* GetInteractionMesh();
	virtual UMeshComponent* GetInteractionMesh_Implementation() { return nullptr; };
};
