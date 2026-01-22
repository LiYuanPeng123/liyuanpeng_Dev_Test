

#include "Trigger/TriggerShape_Sphere.h"
#include "Components/SphereComponent.h"

TSubclassOf<UPrimitiveComponent> UTriggerShape_Sphere::GetPrimitiveComponentClass_Implementation() const
{
	return USphereComponent::StaticClass();
}

void UTriggerShape_Sphere::PostAddComponent_Implementation(UPrimitiveComponent* NewComponent)
{
	Super::PostAddComponent_Implementation(NewComponent);
	if (USphereComponent* SphereComponent = Cast<USphereComponent>(NewComponent))
	{
		SphereComponent->SetSphereRadius(Radius);
	}
}
