// Fill out your copyright notice in the Description page of Project Settings.


#include "CancerAssetSetting.h"

UCancerAssetSetting* UCancerAssetSetting::Get()
{
	return GetMutableDefault<UCancerAssetSetting>();
}

FName UCancerAssetSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerAssetSetting::GetSectionName() const
{
	return FName(TEXT("CancerAsset"));
}
