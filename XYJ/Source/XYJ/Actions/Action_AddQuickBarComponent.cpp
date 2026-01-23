#include "Action_AddQuickBarComponent.h"
#include "CancerInventoryFunctionLibrary.h"
#include "CancerInventory/Public/Components/CancerQuickBarComponent.h"
#include "CancerInventory/Public/CancerQuickBarInstance.h"
#include "Components/CancerInventoryComponent.h"


void UAction_AddQuickBarComponent::GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const
{
	Super::GatherSoftReferences(OutPaths);
}

void UAction_AddQuickBarComponent::PostInitComponent_Implementation()
{
	UCancerQuickBarComponent* QuickBarComponent = Cast<UCancerQuickBarComponent>(GetCreateComponent());
	UCancerInventoryComponent* InventoryComponent = UCancerInventoryFunctionLibrary::GetCancerInventoryComponent(
		GetCreateComponent()->GetOwner());
	check(QuickBarComponent);
	check(InventoryComponent);
}

void UAction_AddQuickBarComponent::FinalizeAfterComponent_Implementation()
{
	Super::FinalizeAfterComponent_Implementation();

	UCancerQuickBarComponent* QuickBarComponent = Cast<UCancerQuickBarComponent>(GetCreateComponent());
	UCancerInventoryComponent* InventoryComponent = UCancerInventoryFunctionLibrary::GetCancerInventoryComponent(
		GetCreateComponent()->GetOwner());
	check(QuickBarComponent);
	check(InventoryComponent);
}
