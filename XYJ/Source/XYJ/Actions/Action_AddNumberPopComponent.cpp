
#include "Action_AddNumberPopComponent.h"

#include "NumberPops/CancerDamagePopStyleNiagara.h"
#include "NumberPops/CancerNumberPopComponent_NiagaraText.h"

void UAction_AddNumberPopComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	if (!Style.IsNull())
	OutPaths.Add(Style.ToSoftObjectPath());
}

void UAction_AddNumberPopComponent::PostInitComponent_Implementation()
{
	UCancerNumberPopComponent_NiagaraText* NumberPopComponent = Cast<UCancerNumberPopComponent_NiagaraText>(GetCreateComponent());
	check(NumberPopComponent);
	NumberPopComponent->SetStyle(Style.Get());
	NumberPopComponent->bIsShowDamage = bIsShowDamage;
	NumberPopComponent->bIsShowCue = bIsShowCue;
	
}
