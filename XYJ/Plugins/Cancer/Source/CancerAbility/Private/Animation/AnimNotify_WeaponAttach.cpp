
#include "Animation/AnimNotify_WeaponAttach.h"
#include "CancerInventoryFunctionLibrary.h"
#include "CancerNativeGamePlayTag.h"
#include "CancerQuickBarInstance.h"
#include "Components/CancerQuickBarComponent.h"
#include "QuickBarIns/QuickBarIns_Weapon.h"

void UAnimNotify_WeaponAttach::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (auto QuickComponent = UCancerInventoryFunctionLibrary::GetCancerQuickBarComponent(MeshComp->GetOwner()))
	{
		if (auto QuickIns  = Cast<UQuickBarIns_Weapon>(QuickComponent->GetQuickBarIns(QuickBar::QuickBar_Weapon)))
		{
			QuickIns->AttachEquipToSlot(InEquipSocket);
		}
	}
}
