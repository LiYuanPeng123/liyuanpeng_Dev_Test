#include "LocomotionData/CancerLocomotionData.h"

#include "CancerAnimNativeTags.h"

DEFINE_LOG_CATEGORY(CancerLocomotionDataLog)

UAnimSequenceBase* UCancerOrientMovementLocomotionData::GetStanceAnim()
{
	return FreeLocomotionAnim.IdleAnim;
}

UAnimSequenceBase* UCancerOrientMovementLocomotionData::GetCrouchAnim()
{
	return FreeLocomotionAnim.CrouchAnim;
}

FStartAnim UCancerOrientMovementLocomotionData::GetStartAnim(const FGameplayTag& GaitTag, bool bStance)
{
	if (bStance)
	{
		if (GaitTag == GaitTag::Walk)
		{
			return FreeLocomotionAnim.Start_Anim;
		}
		else if (GaitTag == GaitTag::Run)
		{
			return FreeLocomotionAnim.Start_RunAnim;
		}
		else
		{
			return FreeLocomotionAnim.Start_SprintAnim;
		}
	}
	else
	{
		if (GaitTag == GaitTag::Walk)
		{
			return FreeLocomotionAnim.C_Start_Anim;
		}
		else
		{
			return FreeLocomotionAnim.C_Start_RunAnim;
		}
	}
}

FLocomotionDirection UCancerOrientMovementLocomotionData::GetCycleAnim(const FGameplayTag& GaitTag, bool bStance)
{
	if (bStance)
	{
		if (GaitTag == GaitTag::Walk)
		{
			return FreeLocomotionAnim.Cycle_Anim;
		}
		else if (GaitTag == GaitTag::Run)
		{
			return FreeLocomotionAnim.Cycle_RunAnim;
		}
		else
		{
			return FreeLocomotionAnim.Cycle_SprintAnim;
		}
	}
	else
	{
		if (GaitTag == GaitTag::Walk)
		{
			return FreeLocomotionAnim.C_Cycle_Anim;
		}
		else
		{
			return FreeLocomotionAnim.C_Cycle_RunAnim;
		}
	}
}

UAnimSequenceBase* UCancerOrientMovementLocomotionData::GetStopAnim(const FGameplayTag& GaitTag, bool bStance)
{
	UAnimSequenceBase* AnimSequence = nullptr;
	if (bStance)
	{
		if (GaitTag == GaitTag::Walk)
		{
			AnimSequence = FreeLocomotionAnim.Stop_Anim;
		}
		else if (GaitTag == GaitTag::Run)
		{
			AnimSequence = FreeLocomotionAnim.Stop_RunAnim;
		}
		else
		{
			AnimSequence = FreeLocomotionAnim.Stop_SprintAnim;
		}
	}
	else
	{
		if (GaitTag == GaitTag::Walk)
		{
			AnimSequence = FreeLocomotionAnim.C_Stop_Anim;
		}
		else
		{
			AnimSequence = FreeLocomotionAnim.C_Stop_RunAnim;
		}
	}
	return AnimSequence;
}

FCancerJumpAnim UCancerOrientMovementLocomotionData::GetJumpAnim(const FGameplayTag& GaitTag)
{
	if (!FreeLocomotionAnim.bSelectJump)
	{
		return FreeLocomotionAnim.JumpAnim;
	}
	if (GaitTag == GaitTag::Walk)
	{
		return FreeLocomotionAnim.JumpAnim;
	}
	else if (GaitTag == GaitTag::Run)
	{
		return FreeLocomotionAnim.RunJumpAnim;
	}
	{
		return FreeLocomotionAnim.SprintJumpAnim;
	}
}

UAnimSequenceBase* UCancerStrafeMovementLocomotionData::GetStanceAnim()
{
	return StrafeStateLocomotion.StrafeStateStanceLocomotion.IdleAnim;
}

UAnimSequenceBase* UCancerStrafeMovementLocomotionData::GetCrouchAnim()
{
	return StrafeStateLocomotion.StrafeStateCrouchLocomotion.IdleAnim;
}

FLocomotionDirection UCancerStrafeMovementLocomotionData::GetStartAnim(const FGameplayTag& GaitTag, bool bStance)
{
	if (bStance)
	{
		if (GaitTag == GaitTag::Walk)
		{
			return StrafeStateLocomotion.StrafeStateStanceLocomotion.Start_Anim;
		}
		else if (GaitTag == GaitTag::Run)
		{
			return StrafeStateLocomotion.StrafeStateStanceLocomotion.Start_RunAnim;
		}
		else
		{
			return StrafeStateLocomotion.StrafeStateStanceLocomotion.Start_SprintAnim;
		}
	}
	else
	{
		if (GaitTag == GaitTag::Walk)
		{
			return StrafeStateLocomotion.StrafeStateCrouchLocomotion.Start_Anim;
		}
		else if (GaitTag == GaitTag::Run)
		{
			return StrafeStateLocomotion.StrafeStateCrouchLocomotion.Start_RunAnim;
		}
		else
		{
			return StrafeStateLocomotion.StrafeStateCrouchLocomotion.Start_SprintAnim;
		}
			
	}
}

FLocomotionDirection UCancerStrafeMovementLocomotionData::GetCycleAnim(const FGameplayTag& GaitTag, bool bStance)
{
	if (bStance)
	{
		if (GaitTag == GaitTag::Walk)
		{
			return StrafeStateLocomotion.StrafeStateStanceLocomotion.Cycle_Anim;
		}
		else if (GaitTag == GaitTag::Run)
		{
			return StrafeStateLocomotion.StrafeStateStanceLocomotion.Cycle_RunAnim;
		}
		else
		{
			return StrafeStateLocomotion.StrafeStateStanceLocomotion.Cycle_SprintAnim;
		}
	}
	else
	{
		if (GaitTag == GaitTag::Walk)
		{
			return StrafeStateLocomotion.StrafeStateCrouchLocomotion.Cycle_Anim;
		}
		else if (GaitTag == GaitTag::Run)
		{
			return StrafeStateLocomotion.StrafeStateCrouchLocomotion.Cycle_RunAnim;
		}
		else
		{
			return StrafeStateLocomotion.StrafeStateCrouchLocomotion.Cycle_SprintAnim;
		}
			
	}
}

FLocomotionDirection UCancerStrafeMovementLocomotionData::GetStopAnim(const FGameplayTag& GaitTag, bool bStance)
{
	if (bStance)
	{
		if (GaitTag == GaitTag::Walk)
		{
			return StrafeStateLocomotion.StrafeStateStanceLocomotion.Stop_Anim;
		}
		else if (GaitTag == GaitTag::Run)
		{
			return StrafeStateLocomotion.StrafeStateStanceLocomotion.Stop_RunAnim;
		}
		else
		{
			return StrafeStateLocomotion.StrafeStateStanceLocomotion.Stop_SprintAnim;
		}
	}
	else
	{
		if (GaitTag == GaitTag::Walk)
		{
			return StrafeStateLocomotion.StrafeStateCrouchLocomotion.Stop_Anim;
		}
		else if (GaitTag == GaitTag::Run)
		{
			return StrafeStateLocomotion.StrafeStateCrouchLocomotion.Stop_RunAnim;
		}
		else
		{
			return StrafeStateLocomotion.StrafeStateCrouchLocomotion.Stop_SprintAnim;
		}
			
	}
}

FCancerJumpAnim UCancerStrafeMovementLocomotionData::GetJumpAnim(const FGameplayTag& GaitTag)
{
	return StrafeStateLocomotion.JumpAnim;
	
}
