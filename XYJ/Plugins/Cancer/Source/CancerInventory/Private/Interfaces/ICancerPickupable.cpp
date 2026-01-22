

#include "Interfaces/ICancerPickupable.h"

#include "Components/CancerInventoryComponent.h"


UPickupableStatics::UPickupableStatics()
	: Super(FObjectInitializer::Get())
{
}

TScriptInterface<ICancerPickupable> UPickupableStatics::GetFirstPickupableFromActor(AActor* Actor)
{
	// If the actor is directly pickupable, return that.
	TScriptInterface<ICancerPickupable> PickupableActor(Actor);
	if (PickupableActor)
	{
		return PickupableActor;
	}

	// If the actor isn't pickupable, it might have a component that has a pickupable interface.
	TArray<UActorComponent*> PickupableComponents = Actor
		                                                ? Actor->GetComponentsByInterface(
			                                                UCancerPickupable::StaticClass())
		                                                : TArray<UActorComponent*>();
	if (PickupableComponents.Num() > 0)
	{
		// Get first pickupable, if the user needs more sophisticated pickup distinction, will need to be solved elsewhere.
		return TScriptInterface<ICancerPickupable>(PickupableComponents[0]);
	}

	return TScriptInterface<ICancerPickupable>();
}

void UPickupableStatics::AddPickupToInventory(UCancerInventoryComponent* InventoryComponent,
                                              TScriptInterface<ICancerPickupable> Pickup)
{
    //@TODO::拾取
    /*if (InventoryComponent && Pickup)
    {
        const FInventoryPickup& PickupInventory = Pickup->GetPickupInventory();

        for (const FPickupTemplate& Template : PickupInventory.Templates)
        {
            InventoryComponent->AddItem(Template.ItemTag, Template.StackCount);
        }

        for (const FPickupInstance& Instance : PickupInventory.Instances)
        {
            InventoryComponent->AddItemIns(Instance.Item,Instance.StackCount);
        }
    }*/
}

FInventoryPickup UPickupableStatics::MakeInventoryPickup(const TArray<FPickupInstance>& Instances,
                                                         const TArray<FPickupTemplate>& Templates)
{
    FInventoryPickup Result;
    Result.Instances = Instances;
    Result.Templates = Templates;
    return Result;
}
