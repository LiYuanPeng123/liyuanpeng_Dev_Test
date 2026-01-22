

#include "Trigger/TriggerShape_Box.h"

#include "Components/BoxComponent.h"

TSubclassOf<UPrimitiveComponent> UTriggerShape_Box::GetPrimitiveComponentClass_Implementation() const
{
	return UBoxComponent::StaticClass();
}

void UTriggerShape_Box::PostAddComponent_Implementation(UPrimitiveComponent* NewComponent)
{
	Super::PostAddComponent_Implementation(NewComponent);
	if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(NewComponent))
	{
		BoxComponent->SetBoxExtent(Extent);
	}
}
