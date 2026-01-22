// Fill out your copyright notice in the Description page of Project Settings.


#include "CancerAISetting.h"

UCancerAISetting* UCancerAISetting::Get()
{
	return GetMutableDefault<UCancerAISetting>();
}

FName UCancerAISetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerAISetting::GetSectionName() const
{
	return FName(TEXT("CancerAI"));
}
