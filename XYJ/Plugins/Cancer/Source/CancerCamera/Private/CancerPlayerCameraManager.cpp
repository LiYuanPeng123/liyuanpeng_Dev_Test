// Copyright Epic Games, Inc. All Rights Reserved.

#include "CancerPlayerCameraManager.h"

#include "Async/TaskGraphInterfaces.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "CancerCameraComponent.h"
#include "CancerUICameraManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CancerPlayerCameraManager)

class FDebugDisplayInfo;

static FName UICameraComponentName(TEXT("UICamera"));

ACancerPlayerCameraManager::ACancerPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    DefaultFOV = CANCER_CAMERA_DEFAULT_FOV;
    ViewPitchMin = CANCER_CAMERA_DEFAULT_PITCH_MIN;
    ViewPitchMax = CANCER_CAMERA_DEFAULT_PITCH_MAX;

    UICamera = CreateDefaultSubobject<UCancerUICameraManagerComponent>(UICameraComponentName);
}

UCancerUICameraManagerComponent* ACancerPlayerCameraManager::GetUICameraComponent() const
{
    return UICamera;
}

void ACancerPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
    // If the UI Camera is looking at something, let it have priority.
    if (UICamera->NeedsToUpdateViewTarget())
    {
        Super::UpdateViewTarget(OutVT, DeltaTime);
        UICamera->UpdateViewTarget(OutVT, DeltaTime);
        return;
    }

    Super::UpdateViewTarget(OutVT, DeltaTime);
}

void ACancerPlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
    check(Canvas);

    FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

    DisplayDebugManager.SetFont(GEngine->GetSmallFont());
    DisplayDebugManager.SetDrawColor(FColor::Yellow);
    DisplayDebugManager.DrawString(FString::Printf(TEXT("CancerPlayerCameraManager: %s"), *GetNameSafe(this)));

    Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

    const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

    if (const UCancerCameraComponent* CameraComponent = UCancerCameraComponent::FindCameraComponent(Pawn))
    {
        CameraComponent->DrawDebug(Canvas);
    }
}

