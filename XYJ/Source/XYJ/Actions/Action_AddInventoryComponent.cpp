#include "Action_AddInventoryComponent.h"
#include "CancerInventory/Public/Components/CancerInventoryComponent.h"



void UAction_AddInventoryComponent::PreInitComponent_Implementation()
{
	UCancerInventoryComponent* InventoryComponent = Cast<UCancerInventoryComponent>(GetCreateComponent());
	check(InventoryComponent);

}

void UAction_AddInventoryComponent::FinalizeAfterComponent_Implementation()
{
	Super::FinalizeAfterComponent_Implementation();
	UCancerInventoryComponent* InventoryComponent = Cast<UCancerInventoryComponent>(GetCreateComponent());
	check(InventoryComponent);
}
