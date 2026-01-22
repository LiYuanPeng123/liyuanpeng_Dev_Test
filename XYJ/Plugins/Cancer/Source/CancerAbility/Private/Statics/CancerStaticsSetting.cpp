// Fill out your copyright notice in the Description page of Project Settings.


#include "Statics/CancerStaticsSetting.h"

FName UCancerStaticsSetting::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}

FName UCancerStaticsSetting::GetSectionName() const
{
	return FName(TEXT("Cancer Statics"));
}