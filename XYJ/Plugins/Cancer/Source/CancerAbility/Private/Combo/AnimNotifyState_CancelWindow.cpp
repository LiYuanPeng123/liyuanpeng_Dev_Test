#include "Combo/AnimNotifyState_CancelWindow.h"
#include "Combo/ComboController.h"

void UAnimNotifyState_CancelWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	if (UComboController* Controller = Owner->FindComponentByClass<UComboController>())
	{
		Controller->NotifyCancelWindowOpen();
	}
}

void UAnimNotifyState_CancelWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	if (UComboController* Controller = Owner->FindComponentByClass<UComboController>())
	{
		Controller->NotifyCancelWindowClose();
	}
}

