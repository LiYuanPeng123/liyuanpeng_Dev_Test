#include "Animation/AnimNotify_DiscardWeapon.h"
#include "CancerInventoryFunctionLibrary.h"
#include "CancerNativeGamePlayTag.h"
#include "CancerQuickBarInstance.h"
#include "Components/CancerQuickBarComponent.h"


void UAnimNotify_DiscardWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (auto QuickComponent = UCancerInventoryFunctionLibrary::GetCancerQuickBarComponent(MeshComp->GetOwner()))
	{
		if (auto QuickIns = QuickComponent->GetQuickBarIns(QuickBar::QuickBar_Weapon))
		{
			FTransform transform = MeshComp->GetOwner()->GetActorTransform();
			if (MeshComp->DoesSocketExist(SocketName))
			{
				transform = MeshComp->GetSocketTransform(SocketName);
			}
			auto Item = QuickIns->DiscardItemInSlot(transform);
		}
	}
}
