// Fill out your copyright notice in the Description page of Project Settings.


#include "CancerTriggerSetting.h"

UCancerTriggerSetting* UCancerTriggerSetting::Get()
{
	return GetMutableDefault<UCancerTriggerSetting>();
}

FName UCancerTriggerSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerTriggerSetting::GetSectionName() const
{
	return FName(TEXT("CancerTrigger"));
}
