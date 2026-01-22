// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CancerCameraAssistInterface.generated.h"

/** */
UINTERFACE(BlueprintType)
class  UCancerCameraAssistInterface : public UInterface
{
	GENERATED_BODY()
};

class CANCERCAMERA_API ICancerCameraAssistInterface
{
	GENERATED_BODY()

public:
	/**
	 * Get the list of actors that we're allowing the camera to penetrate. Useful in 3rd person cameras
	 * when you need the following camera to ignore things like the a collection of view targets, the pawn,
	 * a vehicle..etc.
	 */
	/*** 获取允许摄像机穿透的角色列表。在第三人称摄像机中非常有用，* 当你需要跟随摄像机忽略诸如一系列视图目标、角色、* 车辆等的东西时。*/

	UFUNCTION(BlueprintImplementableEvent,DisplayName="允许摄像机穿透的角色列表")
	void GetIgnoredActorsForCameraPentration(TArray<AActor*>& OutActorsAllowPenetration) const;

	/**
	 * The target actor to prevent penetration on.  Normally, this is almost always the view target, which if
	 * unimplemented will remain true.  However, sometimes the view target, isn't the same as the root actor 
	 * you need to keep in frame.
	 */
	/*** 要防止穿透的目标演员。通常，这几乎总是视图目标，如果* 未实现，将保持为真。然而，有时视图目标并不等同于* 你需要保持在镜头中的根演员。*/
	UFUNCTION(BlueprintImplementableEvent,DisplayName="防止穿透的目标演员")
	AActor* GetCameraPreventPenetrationTarget() const;

	/** Called if the camera penetrates the focal target.  Useful if you want to hide the target actor when being overlapped. */
	/** 当相机穿透焦点目标时调用。如果你想在目标被重叠时隐藏目标角色，这很有用。 */
	UFUNCTION(BlueprintImplementableEvent,DisplayName="相机穿透焦点目标时")
	void OnCameraPenetratingTarget();
};
