
#include "Animation/AnimNotify_RestCombo.h"
#include "Components/CancerHeroComponent.h"

void UAnimNotify_RestCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (auto HeroComponent = UCancerHeroComponent::FindHeroComponent(MeshComp->GetOwner()))
	{
		HeroComponent->ResetCombo();
	}
}

FString UAnimNotify_RestCombo::GetNotifyName_Implementation() const
{
	return TEXT("RestCombo");
}
