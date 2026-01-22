#include "CancerCameraBlueprintLibrary.h"

#include "CancerCameraBehaviorTreeComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Pawn.h"


FMinimalViewInfo UCancerCameraBlueprintLibrary::GetCameraCacheView(APlayerCameraManager* CameraManager)
{
	FMinimalViewInfo ViewInfo;

	if (CameraManager)
	{
		ViewInfo.Location = CameraManager->GetCameraLocation();
		ViewInfo.Rotation = CameraManager->GetCameraRotation();
		ViewInfo.FOV = CameraManager->GetFOVAngle();
	}
	return ViewInfo;
}

UCancerCameraBehaviorTreeComponent* UCancerCameraBlueprintLibrary::K2_GetCancerCameraBehaviorTreeComponent(
	AActor* Actor, bool& bIsValid, TSubclassOf<UCancerCameraBehaviorTreeComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerCameraBehaviorTreeComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerCameraBehaviorTreeComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}
