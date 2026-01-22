#include "Combo/AnimNotifyState_ComboWindowBridge.h"
#include "Combo/ComboController.h"
#include "Combo/ComboInput.h"

void UAnimNotifyState_ComboWindowBridge::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	if (UComboController* Controller = Owner->FindComponentByClass<UComboController>())
	{
		Controller->NotifyComboWindowOpen();
	}
}

void UAnimNotifyState_ComboWindowBridge::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	if (UComboController* Controller = Owner->FindComponentByClass<UComboController>())
	{
		Controller->NotifyComboWindowClose();
	}
	if (UComboInputComponent* Input = Owner->FindComponentByClass<UComboInputComponent>())
	{
		Input->ClearBuffer();
	}
}

