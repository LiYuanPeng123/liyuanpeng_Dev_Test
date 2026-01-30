#include "CancerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerNativeGamePlayTag.h"
#include "Components/CancerMovementComponent.h"
#include "GameFramework/CancerDamageType.h"


ACancerCharacter::ACancerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCancerMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	MovementModeTagMap.Add(MOVE_Walking, Movement_Ground_Walking);
	MovementModeTagMap.Add(MOVE_NavWalking, Movement_Ground_Walking);
	MovementModeTagMap.Add(MOVE_Falling, Movement_Air_Falling);
	MovementModeTagMap.Add(MOVE_Swimming, Movement_Swimming);
	MovementModeTagMap.Add(MOVE_Flying, Movement_Air_Flying);
	MovementModeTagMap.Add(MOVE_Custom, Movement_Custom);
}

void ACancerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	const EMovementMode NewMovementMode = GetCharacterMovement()->MovementMode;
	const uint8 NewCustomMode = GetCharacterMovement()->CustomMovementMode;

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(NewMovementMode, NewCustomMode, true);

	if (OnMovementModeChangedDelegate.IsBound())
	{
		const FGameplayTag PrevTag = GetMovementModeTag(PrevMovementMode, PreviousCustomMode);
		const FGameplayTag NewTag = GetMovementModeTag(NewMovementMode, NewCustomMode);
		OnMovementModeChangedDelegate.Broadcast(this, PrevMovementMode, PreviousCustomMode, PrevTag, NewMovementMode,
		                                        NewCustomMode, NewTag);
	}
}

void ACancerCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(this))
	{
		const FGameplayTag MovementModeTag = GetMovementModeTag(MovementMode, CustomMovementMode);

		if (MovementModeTag.IsValid())
		{
			ASC->SetLooseGameplayTagCount(MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

FGameplayTag ACancerCharacter::GetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode) const
{
	if (MovementMode == MOVE_Custom)
	{
		if (const FGameplayTag* Tag = CustomMovementModeTagMap.Find(CustomMovementMode))
		{
			return *Tag;
		}
	}
	else
	{
		if (const FGameplayTag* Tag = MovementModeTagMap.Find(MovementMode))
		{
			return *Tag;
		}
	}
	return FGameplayTag::EmptyTag;
}
