#include "Combo/ComboGraph.h"

TArray<FTransition> UComboGraph::GetTransitions(UMoveDefinition* Move) const
{
	if (!Move) return TArray<FTransition>();
	if (const FCancerTransitionList* List = Transitions.Find(Move))
	{
		return List->Items;
	}
	return TArray<FTransition>();
}

