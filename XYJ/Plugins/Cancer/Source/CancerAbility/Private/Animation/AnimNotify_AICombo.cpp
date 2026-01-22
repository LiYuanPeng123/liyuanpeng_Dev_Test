#include "Animation/AnimNotify_AICombo.h"
#include "CancerNativeGamePlayTag.h"
#include "Components/CancerHeroComponent.h"
#include "Types/FCancerCombatAnimNotifyHelpers.h"
#include "InputAction.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"

void UAnimNotify_AICombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    if (!InputAction) return;

    // 仅在 AI 控制的角色上执行
    APawn* Pawn = Cast<APawn>(MeshComp->GetOwner());
    if (!Pawn) return;
    AAIController* AIController = Pawn ? Cast<AAIController>(Pawn->GetController()) : nullptr;
    if (!AIController) return;

    if (auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
    {
        FGameplayEventData EventData;
        EventData.EventTag = Tag_Combat_Event_Input_Start;
        EventData.OptionalObject = InputAction;
        ASC->HandleGameplayEvent(Tag_Combat_Event_Input_Start, &EventData);
    	bool bCombo = ASC->HasMatchingGameplayTag(Tag_Combat_State_CombatWindow);
    	FString str = bCombo?"True":"False";
    	UE_LOG(LogTemp,Warning,TEXT("AI连招输入[%s]"),*str);
    }
}

FString UAnimNotify_AICombo::GetNotifyName_Implementation() const
{
	return TEXT("AICombo");;
}
