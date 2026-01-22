

#include "CancerInputHandlerBlueprint.h"

UCancerInputHandlerBlueprint::UCancerInputHandlerBlueprint(const FObjectInitializer& ObjectInitializer)
{
}

#if WITH_EDITOR
bool UCancerInputHandlerBlueprint::SupportedByDefaultBlueprintFactory() const
{
	return false;
}
#endif
