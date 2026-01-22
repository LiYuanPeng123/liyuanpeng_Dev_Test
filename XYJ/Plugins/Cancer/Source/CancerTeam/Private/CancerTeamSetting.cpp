// Fill out your copyright notice in the Description page of Project Settings.


#include "CancerTeamSetting.h"

UCancerTeamSetting* UCancerTeamSetting::Get()
{
	return GetMutableDefault<UCancerTeamSetting>();
}

FName UCancerTeamSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerTeamSetting::GetSectionName() const
{
	return FName(TEXT("CancerTeam"));
}
