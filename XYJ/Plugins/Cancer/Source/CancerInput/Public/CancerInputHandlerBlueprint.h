

#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h"
#include "CancerInputHandlerBlueprint.generated.h"


UCLASS(BlueprintType)
class CANCERINPUT_API UCancerInputHandlerBlueprint : public UBlueprint
{
	GENERATED_BODY()
public:

	UCancerInputHandlerBlueprint(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR

	// UBlueprint interface
	virtual bool SupportedByDefaultBlueprintFactory() const override;
	// End of UBlueprint interface
    
#endif
};