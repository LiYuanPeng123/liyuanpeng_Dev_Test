
#include "ANS/CancerMontageBlendOut.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


DEFINE_LOG_CATEGORY(MontageBlendOutLog)

void UCancerMontageBlendOut::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	if (!MeshComp||!MeshComp->GetWorld()->IsGameWorld())return;
	
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	UAnimMontage* AnimMontage = Cast<UAnimMontage>(Animation);
	bool bShouldBlendOut = false;
	if (BlendOut == EBlendOutEnum::ForceBlendOut)
	{
		bShouldBlendOut = true;
	}
	else if (BlendOut == EBlendOutEnum::WithInput)
	{
		bool IsZero = UKismetMathLibrary::EqualEqual_VectorVector(
			Character->GetCharacterMovement()->GetCurrentAcceleration(),
			FVector::Zero(), 0.1f);
		bShouldBlendOut = !IsZero;
	}
	else if (BlendOut == EBlendOutEnum::HasInput)
	{
		bool IsZero = UKismetMathLibrary::EqualEqual_VectorVector(
			Character->GetCharacterMovement()->GetCurrentAcceleration(),
			FVector::Zero(), 0.1f);
		bShouldBlendOut = IsZero;
	}
	else if (BlendOut == EBlendOutEnum::Falling)
	{
		bool IsFalling = Character->GetCharacterMovement()->IsFalling();
		bShouldBlendOut = IsFalling;
	}
	else if (BlendOut == EBlendOutEnum::HasTag)
	{
		if (auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
		{
			bShouldBlendOut = ASC->HasMatchingGameplayTag(BlendOutTag);
		}
	}

	if (bShouldBlendOut)
	{
		FMontageBlendSettings BlendOutSettings;
		BlendOutSettings.BlendProfile = const_cast<UBlendProfile*>(AnimInstance->
			GetBlendProfileByName(BlendProfileName));
		BlendOutSettings.BlendMode = BlendMode;
		BlendOutSettings.Blend = Blend;
		AnimInstance->Montage_StopWithBlendSettings(BlendOutSettings, AnimMontage);
/*#define IF_WITH_EDITOR()*/
#if WITH_EDITOR 
		FString BlendOutName = StaticEnum<EBlendOutEnum>()->GetNameStringByValue(static_cast<int64>(BlendOut));
		UE_LOG(MontageBlendOutLog, Display, TEXT("%s <---> %s"), *BlendOutName, *AnimMontage->GetName());
#endif
/*#undef IF_WITH_EDITOR*/
	}
}

FString UCancerMontageBlendOut::GetNotifyName_Implementation() const
{
	return TEXT("蒙太奇打断");
}
