

#include "Animation/AnimNotify_SetComboIndex.h"

#include "Components/CancerHeroComponent.h"

UAnimNotify_SetComboIndex::UAnimNotify_SetComboIndex()
{
#if WITH_EDITORONLY_DATA
	// Ninja Bear Green! :D
	NotifyColor = FColor(0, 255, 0);
#endif
}

void UAnimNotify_SetComboIndex::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (auto HeroComponent = UCancerHeroComponent::FindHeroComponent(MeshComp->GetOwner()))
	{
		HeroComponent->ComboIndex = ComboIndex;
	}
}

FString UAnimNotify_SetComboIndex::GetNotifyName_Implementation() const
{
	return 	FString::Printf(TEXT(" ComboIndex:[%d]"), ComboIndex);
}
