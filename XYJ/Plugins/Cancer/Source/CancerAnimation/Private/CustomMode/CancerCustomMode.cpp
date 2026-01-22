// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomMode/CancerCustomMode.h"

bool UCancerCustomMode::TryEnterCustomMode_Implementation()
{
	return false;
}

bool UCancerCustomMode::CanEnterCustomMode_Implementation()
{
	return false;
}

void UCancerCustomMode::EnterCustomMode_Implementation()
{
	IsActivate = true;
}

void UCancerCustomMode::UpdateCustomMode_Implementation()
{
	PhysCustom();
}

void UCancerCustomMode::ExitCustomMode_Implementation()
{
	IsActivate = false;
}

void UCancerCustomMode::PhysCustom_Implementation()
{
}

int32 UCancerCustomMode::GetCustomMode_Implementation()
{
	return StaticEnum<ECancerCustomMovementMode>()->GetIndexByValue(static_cast<int64>(CustomMode));
}

FString UCancerCustomMode::GetCustomModeDisplayName_Implementation()
{
	return StaticEnum<ECancerCustomMovementMode>()->GetNameStringByValue(static_cast<int64>(CustomMode));
}
