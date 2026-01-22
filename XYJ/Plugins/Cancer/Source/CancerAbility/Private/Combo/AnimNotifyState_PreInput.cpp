#include "Combo/AnimNotifyState_PreInput.h"
#include "Combo/ComboController.h"

void UAnimNotifyState_PreInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	if (UComboController* Controller = Owner->FindComponentByClass<UComboController>())
	{
		Controller->NotifyPreInputOpen();
	}
}

void UAnimNotifyState_PreInput::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	if (UComboController* Controller = Owner->FindComponentByClass<UComboController>())
	{
		Controller->NotifyPreInputClose();
	}
}

