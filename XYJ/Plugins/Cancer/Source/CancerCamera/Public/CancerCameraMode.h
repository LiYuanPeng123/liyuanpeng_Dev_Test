#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "CancerCameraMode.generated.h"

class UCancerCameraComponent;

UENUM(BlueprintType)
enum class ECancerCameraModeBlendFunction : uint8
{
	// Does a simple linear interpolation.
	Linear UMETA(DisplayName = "线性"),

	// Immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by the exponent.
	EaseIn UMETA(DisplayName = "立即加速，但会平稳地减速直至抵达目标位置"),

	// Smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by the exponent.
	EaseOut UMETA(DisplayName = "平稳加速，但不会减速直至抵达目标位置"),

	// Smoothly accelerates and decelerates.  Ease amount controlled by the exponent.
	EaseInOut  UMETA(DisplayName = "能平稳地加速和减速"),

	COUNT UMETA(Hidden)
};

struct FCancerCameraModeView
{
public:
	FCancerCameraModeView();

	void Blend(const FCancerCameraModeView& Other, float OtherWeight);

public:
	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

UCLASS(Abstract, NotBlueprintable,DefaultToInstanced,EditInlineNew)
class CANCERCAMERA_API UCancerCameraMode : public UObject
{
	GENERATED_BODY()

public:
	UCancerCameraMode();

	UCancerCameraComponent* GetCancerCameraComponent() const;

	virtual UWorld* GetWorld() const override;

	void SetCameraOwner(AActor*InActor);
	
	AActor* GetTargetActor() const;

	const FCancerCameraModeView& GetCameraModeView() const { return View; }

	// Called when this camera mode is activated on the camera mode stack.
	virtual void OnActivation()
	{
	};

	// Called when this camera mode is deactivated on the camera mode stack.
	virtual void OnDeactivation()
	{
	};

	void UpdateCameraMode(float DeltaTime);

	float GetBlendTime() const { return BlendTime; }
	float GetBlendWeight() const { return BlendWeight; }
	void SetBlendWeight(float Weight);

	FGameplayTag GetCameraTypeTag() const
	{
		return CameraTypeTag;
	}

	virtual void DrawDebug(UCanvas* Canvas) const;

protected:
	virtual FVector GetPivotLocation() const;
	virtual FRotator GetPivotRotation() const;

	virtual void UpdateView(float DeltaTime);
	
	virtual void UpdateBlending(float DeltaTime);

protected:

	UPROPERTY()
	AActor* CameraOwner;
	// A tag that can be queried by gameplay code that cares when a kind of camera mode is active
	// without having to ask about a specific mode (e.g., when aiming downsights to get more accuracy)
	UPROPERTY(EditAnywhere, Category = "Blending")
	FGameplayTag CameraTypeTag;

	// View output produced by the camera mode.
	FCancerCameraModeView View;

	// The horizontal field of view (in degrees).
	UPROPERTY(EditAnywhere, Category = "View",
		Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	// Minimum view pitch (in degrees).
	UPROPERTY(EditAnywhere, Category = "View",
		Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	// Maximum view pitch (in degrees).
	UPROPERTY(EditAnywhere, Category = "View",
		Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	// How long it takes to blend in this mode.
	UPROPERTY(EditAnywhere, Category = "Blending")
	float BlendTime;

	// Function used for blending.
	UPROPERTY(EditAnywhere, Category = "Blending")
	ECancerCameraModeBlendFunction BlendFunction;

	// Exponent used by blend functions to control the shape of the curve.
	UPROPERTY(EditAnywhere, Category = "Blending")
	float BlendExponent;

	// Linear blend alpha used to determine the blend weight.
	float BlendAlpha;

	// Blend weight calculated using the blend alpha and function.
	float BlendWeight;

protected:
	/** If true, skips all interpolation and puts camera in ideal location.  Automatically set to false next frame. */
	UPROPERTY(transient)
	uint32 bResetInterpolation : 1;
};


/**
 * UCancerCameraModeStack
 *
 *	Stack used for blending camera modes.
 */
UCLASS()
class UCancerCameraModeStack : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AActor* CameraOwner;
	UCancerCameraModeStack();

	void ActivateStack();
	void DeactivateStack();

	bool IsStackActivate() const { return bIsActive; }

	void PushCameraMode(UCancerCameraMode* CameraIns);
	void PushCameraModeTemplate(UCancerCameraMode* CameraIns);
	void PushCameraModeClass(TSubclassOf<UCancerCameraMode> CameraModeClass);

	bool EvaluateStack(float DeltaTime, FCancerCameraModeView& OutCameraModeView);

	void DrawDebug(UCanvas* Canvas) const;

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:
	UCancerCameraMode* GetCameraModeInstance(TSubclassOf<UCancerCameraMode> CameraModeClass);

	void UpdateStack(float DeltaTime);
	void BlendStack(FCancerCameraModeView& OutCameraModeView) const;

protected:
	bool bIsActive;

	UPROPERTY()
	TArray<TObjectPtr<UCancerCameraMode>> CameraModeInstances;

	UPROPERTY()
	TArray<TObjectPtr<UCancerCameraMode>> CameraModeStack;
};


USTRUCT(BlueprintType,Blueprintable)
struct FCancerCameraModeInstanced
{
	GENERATED_BODY()
	UPROPERTY(Instanced,EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UCancerCameraMode> CameraModeInstance;
	FCancerCameraModeInstanced(){CameraModeInstance = nullptr;}
};
