
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerConstantsFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS(Meta = (BlueprintThreadSafe))
class CANCERANIMATION_API UCancerConstantsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& RootBoneName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& PelvisBoneName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HeadBoneName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& Spine03BoneName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftBoneName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightBoneName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HandLeftGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HandRightGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftIkBoneName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightIkBoneName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightVirtualBoneName();

	// Animation Slots

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TransitionSlotName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TurnInPlaceStandingSlotName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TurnInPlaceCrouchingSlotName();

	// Layering Animation Curves

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftLocCancerpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightLocCancerpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHandLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHandRightCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerPelvisCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerPelvisSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerLegsCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerLegsSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HandLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HandRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& ViewBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& AllowAimingCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HipsDirectionLockCurveName();

	// Pose Animation Curves

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseGaitCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseMovingCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseStandingCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseCrouchingCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseGroundedCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseInAirCurveName();

	// Feet Animation Curves

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftLockCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightLockCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootPlantedCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FeetCrossingCurveName();

	// Other Animation Curves

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationYawSpeedCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationYawOffsetCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& AllowTransitionsCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& SprintBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& GroundPredictionBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootstepSoundBlockCurveName();

	// Debug

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CurvesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& StateDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& ShapesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& TracesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "Cancer|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& MantlingDebugDisplayName();
};

inline const FName& UCancerConstantsFunctionLibrary::RootBoneName()
{
	static const FName Name{TEXTVIEW("root")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::PelvisBoneName()
{
	static const FName Name{TEXTVIEW("pelvis")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::HeadBoneName()
{
	static const FName Name{TEXTVIEW("head")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::Spine03BoneName()
{
	static const FName Name{TEXTVIEW("spine_03")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootLeftBoneName()
{
	static const FName Name{TEXTVIEW("foot_l")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootRightBoneName()
{
	static const FName Name{TEXTVIEW("foot_r")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::HandLeftGunVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB hand_l_to_ik_hand_gun")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::HandRightGunVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB hand_r_to_ik_hand_gun")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootLeftIkBoneName()
{
	static const FName Name{TEXTVIEW("ik_foot_l")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootRightIkBoneName()
{
	static const FName Name{TEXTVIEW("ik_foot_r")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootLeftVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB foot_l")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootRightVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB foot_r")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::TransitionSlotName()
{
	static const FName Name{TEXTVIEW("Transition")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::TurnInPlaceStandingSlotName()
{
	static const FName Name{TEXTVIEW("TurnInPlaceStanding")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::TurnInPlaceCrouchingSlotName()
{
	static const FName Name{TEXTVIEW("TurnInPlaceCrouching")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerHeadCurveName()
{
	static const FName Name{TEXTVIEW("LayerHead")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerHeadAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerHeadAdditive")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerHeadSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerHeadSlot")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerArmLeftCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeft")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerArmLeftAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeftAdditive")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerArmLeftLocCancerpaceCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeftLocCancerpace")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerArmLeftSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeftSlot")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerArmRightCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRight")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerArmRightAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRightAdditive")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerArmRightLocCancerpaceCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRightLocCancerpace")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerArmRightSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRightSlot")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerHandLeftCurveName()
{
	static const FName Name{TEXTVIEW("LayerHandLeft")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerHandRightCurveName()
{
	static const FName Name{TEXTVIEW("LayerHandRight")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerSpineCurveName()
{
	static const FName Name{TEXTVIEW("LayerSpine")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerSpineAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerSpineAdditive")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerSpineSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerSpineSlot")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerPelvisCurveName()
{
	static const FName Name{TEXTVIEW("LayerPelvis")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerPelvisSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerPelvisSlot")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerLegsCurveName()
{
	static const FName Name{TEXTVIEW("LayerLegs")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::LayerLegsSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerLegsSlot")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::HandLeftIkCurveName()
{
	static const FName Name{TEXTVIEW("HandLeftIk")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::HandRightIkCurveName()
{
	static const FName Name{TEXTVIEW("HandRightIk")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::ViewBlockCurveName()
{
	static const FName Name{TEXTVIEW("ViewBlock")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::AllowAimingCurveName()
{
	static const FName Name{TEXTVIEW("AllowAiming")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::HipsDirectionLockCurveName()
{
	static const FName Name{TEXTVIEW("HipsDirectionLock")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::PoseGaitCurveName()
{
	static const FName Name{TEXTVIEW("PoseGait")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::PoseMovingCurveName()
{
	static const FName Name{TEXTVIEW("PoseMoving")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::PoseStandingCurveName()
{
	static const FName Name{TEXTVIEW("PoseStanding")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::PoseCrouchingCurveName()
{
	static const FName Name{TEXTVIEW("PoseCrouching")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::PoseGroundedCurveName()
{
	static const FName Name{TEXTVIEW("PoseGrounded")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::PoseInAirCurveName()
{
	static const FName Name{TEXTVIEW("PoseInAir")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootLeftIkCurveName()
{
	static const FName Name{TEXTVIEW("FootLeftIk")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootLeftLockCurveName()
{
	static const FName Name{TEXTVIEW("FootLeftLock")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootRightIkCurveName()
{
	static const FName Name{TEXTVIEW("FootRightIk")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootRightLockCurveName()
{
	static const FName Name{TEXTVIEW("FootRightLock")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootPlantedCurveName()
{
	static const FName Name{TEXTVIEW("FootPlanted")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FeetCrossingCurveName()
{
	static const FName Name{TEXTVIEW("FeetCrossing")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::RotationYawSpeedCurveName()
{
	static const FName Name{TEXTVIEW("RotationYawSpeed")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::RotationYawOffsetCurveName()
{
	static const FName Name{TEXTVIEW("RotationYawOffset")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::AllowTransitionsCurveName()
{
	static const FName Name{TEXTVIEW("AllowTransitions")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::SprintBlockCurveName()
{
	static const FName Name{TEXTVIEW("SprintBlock")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::GroundPredictionBlockCurveName()
{
	static const FName Name{TEXTVIEW("GroundPredictionBlock")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::FootstepSoundBlockCurveName()
{
	static const FName Name{TEXTVIEW("FootstepSoundBlock")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::CurvesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("Cancer.Curves")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::StateDebugDisplayName()
{
	static const FName Name{TEXTVIEW("Cancer.State")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::ShapesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("Cancer.Shapes")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::TracesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("Cancer.Traces")};
	return Name;
}

inline const FName& UCancerConstantsFunctionLibrary::MantlingDebugDisplayName()
{
	static const FName Name{TEXTVIEW("Cancer.Mantling")};
	return Name;
}
