
#include "CancerUICameraManagerComponent.h"

#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "CancerPlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CancerUICameraManagerComponent)

class AActor;
class FDebugDisplayInfo;

UCancerUICameraManagerComponent* UCancerUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (ACancerPlayerCameraManager* PCCamera = Cast<ACancerPlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCCamera->GetUICameraComponent();
		}
	}
	return nullptr;
}

UCancerUICameraManagerComponent::UCancerUICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "showdebug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

void UCancerUICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UCancerUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<ACancerPlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool UCancerUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void UCancerUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
	
}

void UCancerUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
}
