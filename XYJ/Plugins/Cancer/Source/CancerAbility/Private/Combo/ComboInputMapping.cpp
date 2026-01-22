#include "Combo/ComboInputMapping.h"

FGameplayTag UComboInputMapping::MapPhysicalToLogical(const FGameplayTag& Physical) const
{
	if (const FGameplayTag* Found = PhysicalToLogical.Find(Physical))
	{
		return *Found;
	}
	return Physical;
}

FGameplayTag UComboInputMapping::TryResolveChord(const TArray<FGameplayTag>& PressedInWindow) const
{
	for (const FComboChord& Chord : Chords)
	{
		bool AllPresent = true;
		for (const FGameplayTag& Required : Chord.Inputs)
		{
			if (!PressedInWindow.Contains(Required))
			{
				AllPresent = false;
				break;
			}
		}
		if (AllPresent)
		{
			return Chord.Output;
		}
	}
	return FGameplayTag();
}

