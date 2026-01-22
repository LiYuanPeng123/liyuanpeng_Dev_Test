#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CancerActionInterface.generated.h"

UINTERFACE()
class UCancerActionInterface : public UInterface
{
	GENERATED_BODY()
};

class CANCERACTIONCREATOR_API ICancerActionInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void HandleOnActionCompleted();
};
