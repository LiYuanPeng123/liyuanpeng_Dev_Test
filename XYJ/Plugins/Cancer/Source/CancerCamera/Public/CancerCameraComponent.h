
#pragma once

#include "CoreMinimal.h"
#include "CancerCameraMode_ThirdPerson.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "CancerCameraComponent.generated.h"

struct FGameplayTag;
class UCancerCameraModeStack;
class UCancerCameraMode;
DECLARE_DELEGATE_RetVal(TSubclassOf<UCancerCameraMode>, FCancerCameraModeDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CANCERCAMERA_API UCancerCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UCancerCameraComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the camera component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Cancer|Camera")
	static UCancerCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UCancerCameraComponent>() : nullptr); }

	UFUNCTION(BlueprintCallable, Category = "Cancer|Camera")
	void PushCameraMode(TSubclassOf<UCancerCameraMode> CameraModeClass);

	UFUNCTION(BlueprintCallable, Category = "Cancer|Camera")
	void PushCameraModeTemplate(UCancerCameraMode* CameraIns);

	UFUNCTION(BlueprintCallable, Category = "Cancer|Camera")
	void PushCameraModeWithTargetLoc(TSubclassOf<UCancerCameraMode_ThirdPerson> CameraModeClass,FVector TargetLoc);
	
	
	UFUNCTION(BlueprintCallable, Category = "Cancer|Camera")
	// Returns the target actor that the camera is looking at.
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	// Delegate used to query for the best camera mode.
	FCancerCameraModeDelegate DetermineCameraModeDelegate;

	// Add an offset to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }

	virtual void DrawDebug(UCanvas* Canvas) const;

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:

	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
	virtual void UpdateCameraModes();

protected:

	// Stack used to blend the camera modes.
	UPROPERTY()
	TObjectPtr<UCancerCameraModeStack> CameraModeStack;

	// Offset applied to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	float FieldOfViewOffset;
};
