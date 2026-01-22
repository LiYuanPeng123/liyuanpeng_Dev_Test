// Fill out your copyright notice in the Description page of Project Settings.


#include "CancerInputSettings.h"

#include "CancerNativeGamePlayTag.h"
#include "InputTriggers.h"

UCancerInputSettings::UCancerInputSettings()
{
	TrackedEvents = {
		ETriggerEvent::Started,
		ETriggerEvent::Triggered,
		ETriggerEvent::Ongoing,
		ETriggerEvent::Completed,
		ETriggerEvent::Canceled,
	};

	bShowScreenDebugMessages = false;
	DebugMessageDuration = 5.f;
	DebugMessageColor = FColor::Emerald;
	DefaultGameInputTag = InputLayer::Game;
	InputLayerDataArray.Add(FInputLayerData(InputLayer::UI, true));
	InputLayerDataArray.Add(FInputLayerData(InputLayer::Game));
}

FName UCancerInputSettings::GetCategoryName() const
{
	return FName(TEXT("Cancer"));
}


FName UCancerInputSettings::GetSectionName() const
{
	return FName(TEXT("CancerInput"));
}

UCancerInputSettings* UCancerInputSettings::Get()
{
	return GetMutableDefault<UCancerInputSettings>();
}
