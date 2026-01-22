

#include "Trigger/TriggerShape_Base.h"

#include "CancerTriggerSetting.h"

TSubclassOf<UPrimitiveComponent> UTriggerShape_Base::GetPrimitiveComponentClass_Implementation() const
{
	return UPrimitiveComponent::StaticClass();
}

void UTriggerShape_Base::PostAddComponent_Implementation(UPrimitiveComponent* NewComponent)
{
	NewComponent->SetGenerateOverlapEvents(true);
	NewComponent->SetCollisionProfileName(GetDefault<UCancerTriggerSetting>()->TriggerProfileName.Name);
	NewComponent->SetCollisionResponseToChannel(ECC_Visibility,ECR_Ignore);
}
