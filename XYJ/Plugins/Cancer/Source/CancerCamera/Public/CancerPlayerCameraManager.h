#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CancerPlayerCameraManager.generated.h"

#define CANCER_CAMERA_DEFAULT_FOV		(80.0f)
#define CANCER_CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define CANCER_CAMERA_DEFAULT_PITCH_MAX	(89.0f)

class UCancerUICameraManagerComponent;

UCLASS(notplaceable)
class CANCERCAMERA_API ACancerPlayerCameraManager : public APlayerCameraManager
{
    GENERATED_BODY()

public:

    ACancerPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

    UCancerUICameraManagerComponent* GetUICameraComponent() const;

protected:

    virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

    virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
    /** The UI Camera Component, controls the camera when UI is doing something important that gameplay doesn't get priority over. */
    UPROPERTY(Transient)
    TObjectPtr<UCancerUICameraManagerComponent> UICamera;
};