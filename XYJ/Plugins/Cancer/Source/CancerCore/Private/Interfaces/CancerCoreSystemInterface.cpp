
#include "Interfaces/CancerCoreSystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerNativeGamePlayTag.h"
#include "GameFramework/Character.h"

USkeletalMeshComponent* ICancerCoreSystemInterface::GetCombatMesh_Implementation() const
{
	const AActor* SelfActor = Cast<AActor>(this);
	if (IsValid(SelfActor))
	{
		// First Attempt: Find a component with the combat Tag.
		//
		const FName CombatTag = Tag_Combat_Component_Mesh.GetTag().GetTagName();
		USkeletalMeshComponent* Mesh = SelfActor->FindComponentByTag<USkeletalMeshComponent>(CombatTag);

		// Second Attempt: Use the tag configured in the Ability System Component, if not "NONE".
		//
		if (!IsValid(Mesh))
		{
			const UAbilitySystemComponent* AbilityComponent =
				UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SelfActor);
			if (IsValid(AbilityComponent) && AbilityComponent->AffectedAnimInstanceTag != NAME_None)
			{
				const FName AbilityTag = AbilityComponent->AffectedAnimInstanceTag;
				Mesh = SelfActor->FindComponentByTag<USkeletalMeshComponent>(AbilityTag);
			}
		}

		// Third Attempt: Cast to a character and return the main mesh.
		//
		if (!IsValid(Mesh))
		{
			const ACharacter* Character = Cast<ACharacter>(SelfActor);
			Mesh = IsValid(Character) ? Character->GetMesh() : nullptr;
		}

		return Mesh;
	}

	return nullptr;
}

UAnimInstance* ICancerCoreSystemInterface::GetCombatAnimInstance_Implementation() const
{
	const AActor* SelfActor = Cast<AActor>(this);
	if (IsValid(SelfActor))
	{
		const USkeletalMeshComponent* Mesh = Execute_GetCombatMesh(SelfActor);
		return IsValid(Mesh) ? Mesh->GetAnimInstance() : nullptr;
	}

	return nullptr;
}

USceneComponent* ICancerCoreSystemInterface::GetCombatForwardReference_Implementation() const
{
	const AActor* SelfActor = Cast<AActor>(this);
	if (IsValid(SelfActor))
	{
		const FName ForwardTag = Tag_Combat_Component_Forwardreference.GetTag().GetTagName();
		return SelfActor->FindComponentByTag<USceneComponent>(ForwardTag);
	}

	return nullptr;
}
