
#include "AbilitySystem/Effects/CancerEffect_Cure.h"
#include "CancerNativeGamePlayTag.h"
#include "AbilitySystem/Attributes/CancerHealthSet.h"

UCancerEffect_Cure::UCancerEffect_Cure()
{

	//添加修饰符
	{
		FSetByCallerFloat SetByCaller;
		SetByCaller.DataTag = SetByCaller_Cure;

		FGameplayModifierInfo Mod;
		Mod.Attribute = UCancerHealthSet::GetHealthAttribute();
		Mod.ModifierOp = EGameplayModOp::Additive;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
		Modifiers.Add(Mod);
	}
}
