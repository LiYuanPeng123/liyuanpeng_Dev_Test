
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LockingTargetInterface.generated.h"


class UCancerLockingTargetComponent;

UINTERFACE()
class ULockingTargetInterface : public UInterface
{
	GENERATED_BODY()
};

class CANCERLOCKTARGET_API ILockingTargetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UCancerLockingTargetComponent* GetLockingTargetComponent();
};
