
#include "Providers/CancerCombatDirectionalAnimationProvider.h"

UCancerCombatDirectionalAnimationProvider::UCancerCombatDirectionalAnimationProvider()
{
	bUseContextDirections = true;
	DefaultSectionName = "Backward";
	InitializeDirections(AnimationDirections);	
}

FName UCancerCombatDirectionalAnimationProvider::GetSectionName_Implementation(
	UCancerGameplayAbility* CombatAbility) const
{
	float Angle;
	if (CalculateAnimationAngle(CombatAbility, Angle))
	{
		const int32 Index = AnimationDirections.IndexOfByPredicate([Angle](const FCombatAnimationDirection& D)
		{
			return D.InRange(Angle);
		});
		if (Index != INDEX_NONE)
		{
			UE_LOG(LogTemp,Warning,TEXT("角度:%f,片段名称:%s"),Angle, *AnimationDirections[Index].SectionName.ToString());
			return AnimationDirections[Index].SectionName;
		}
	}

	return DefaultSectionName;
}

bool UCancerCombatDirectionalAnimationProvider::CalculateAnimationAngle_Implementation(
	const UCancerGameplayAbility* CombatAbility, float& OutAngle) const
{
	return false;
}

void UCancerCombatDirectionalAnimationProvider::InitializeDirections(TArray<FCombatAnimationDirection>& DirectionsArray)
{
	static constexpr int32 DirectionCount = 10;

	static const FName Forward = "Forward", Backward = "Backward", L045 = "Left_045", L090 = "Left_090",
	                   L135 = "Left_135", R045 = "Right_045", R090 = "Right_090", R135 = "Right_135";

	DirectionsArray.Reset(DirectionCount);
	DirectionsArray.Add(FCombatAnimationDirection(0.f, 35.f, Forward));
	DirectionsArray.Add(FCombatAnimationDirection(-35.f, 0.f, Forward));
	DirectionsArray.Add(FCombatAnimationDirection(-74.f, -36.f, L045));
	DirectionsArray.Add(FCombatAnimationDirection(-125.f, -75.f, L090));
	DirectionsArray.Add(FCombatAnimationDirection(-164.f, -126.f, L135));
	DirectionsArray.Add(FCombatAnimationDirection(36.f, 74.f, R045));
	DirectionsArray.Add(FCombatAnimationDirection(75.f, 125.f, R090));
	DirectionsArray.Add(FCombatAnimationDirection(126.f, 164.f, R135));
	DirectionsArray.Add(FCombatAnimationDirection(165.f, 180.f, Backward));
	DirectionsArray.Add(FCombatAnimationDirection(-180.f, -165.f, Backward));
}
