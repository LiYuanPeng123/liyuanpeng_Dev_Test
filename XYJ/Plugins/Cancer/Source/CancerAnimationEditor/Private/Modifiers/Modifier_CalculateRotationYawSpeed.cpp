// Fill out your copyright notice in the Description page of Project Settings.


#include "Modifiers/Modifier_CalculateRotationYawSpeed.h"

void UModifier_CalculateRotationYawSpeed::OnApply_Implementation(UAnimSequence* Sequence)
{
	Super::OnApply_Implementation(Sequence);
	if (UAnimationBlueprintLibrary::DoesCurveExist(Sequence, RotationYawSpeedCurveName, ERawCurveTrackTypes::RCT_Float))
	{
		UAnimationBlueprintLibrary::RemoveCurve(Sequence, RotationYawSpeedCurveName);
	}

	UAnimationBlueprintLibrary::AddCurve(Sequence, RotationYawSpeedCurveName);

	const auto* DataModel{Sequence->GetDataModel()};
	const auto FrameRate{Sequence->GetSamplingFrameRate().AsDecimal()};

	UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, RotationYawSpeedCurveName, 0.0f, 0.0f);

	for (auto i{1}; i < Sequence->GetNumberOfSampledKeys(); i++)
	{
		auto CurrentPoseTransform{
			DataModel->GetBoneTrackTransform(RootBoneName, i + (Sequence->RateScale >= 0.0f ? -1 : 0))
		};

		auto NextPoseTransform{
			DataModel->GetBoneTrackTransform(RootBoneName, i + (Sequence->RateScale >= 0.0f ? 0 : -1))
		};

		UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, RotationYawSpeedCurveName, Sequence->GetTimeAtFrame(i),
													 UE_REAL_TO_FLOAT(
														 (NextPoseTransform.Rotator().Yaw - CurrentPoseTransform.Rotator().Yaw) *
														 FMath::Abs(Sequence->RateScale) * FrameRate));
	}
}
