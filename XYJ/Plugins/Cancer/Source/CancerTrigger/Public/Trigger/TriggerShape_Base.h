
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TriggerShape_Base.generated.h"

/**
 * 
 */
UCLASS(Abstract, EditInlineNew)
class CANCERTRIGGER_API UTriggerShape_Base : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UPrimitiveComponent> GetPrimitiveComponentClass() const;

	UFUNCTION(BlueprintNativeEvent)
	void PostAddComponent(UPrimitiveComponent* NewComponent);
};
