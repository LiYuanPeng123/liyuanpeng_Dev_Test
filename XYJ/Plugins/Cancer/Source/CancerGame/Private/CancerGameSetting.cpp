// Fill out your copyright notice in the Description page of Project Settings.


#include "CancerGameSetting.h"

UCancerGameSetting* UCancerGameSetting::Get()
{
	return GetMutableDefault<UCancerGameSetting>();
}

FName UCancerGameSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerGameSetting::GetSectionName() const
{
	return FName(TEXT("CancerGame"));
}
