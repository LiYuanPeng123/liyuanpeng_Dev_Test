#include "CancerIndicatorSubsystem.h"

void UCancerIndicatorSubsystem::AddIndicator(UCancerIndicatorSourceComponent* SourceIndicatorComponent)
{
	IndicatorSourceArray.Add(SourceIndicatorComponent);
	OnIndicatorChange.Broadcast(SourceIndicatorComponent,true);
}

void UCancerIndicatorSubsystem::RemoveIndicator(UCancerIndicatorSourceComponent* SourceIndicatorComponent)
{
	IndicatorSourceArray.Remove(SourceIndicatorComponent);
	OnIndicatorChange.Broadcast(SourceIndicatorComponent,false);
}

TArray<UCancerIndicatorSourceComponent*> UCancerIndicatorSubsystem::GetIndicatorComponents()
{
	return IndicatorSourceArray;
}
