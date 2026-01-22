#include "Evaluator_Base.h"

class UWorld* UEvaluator_Base::GetWorld() const
{
	if (this->IsTemplate())
	{
		return GWorld ? GWorld->GetWorld() : nullptr;
	}
	if (UObject* Outer = GetOuter())
	{
		if (UWorld* World = Outer->GetWorld())
		{
			if (World->IsGameWorld())
			{
				return World;
			}
		}
	}
	for (FWorldContext WorldContext : GEngine->GetWorldContexts())
	{
		if (WorldContext.World()->IsGameWorld())
		{
			return WorldContext.World();
		}
	}
	return nullptr;
}

double UEvaluator_Base::GetValue(UObject* WorldContextObject)
{
	return 0;
}