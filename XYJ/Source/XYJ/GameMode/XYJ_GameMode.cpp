#include "XYJ_GameMode.h"

#include "CancerActionLibrary.h"
#include "CancerActorSubsystem.h"
#include "CancerAssetManager.h"
#include "CancertGlobalAbilitySystem.h"
#include "XYJWorldSetting.h"
#include "XYJ/Character/XYJ_Character.h"
#include "CancerActionCreator/Public/Action_DataAssetTemplate.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "XYJ/AI/AAICreator.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"


AXYJ_GameMode::AXYJ_GameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

UAction_DataAsset* AXYJ_GameMode::GetEffectivePawnData() const
{
	if (auto WorldSetting = Cast<AXYJWorldSetting>(GetWorldSettings()))
	{
		if (auto Data = UCancerAssetManager::Get().GetAsset(WorldSetting->PawnData))
		{
			return Data;
		}
	}
	return UCancerAssetManager::Get().GetAsset(DefaultPawnData);
}

TSoftObjectPtr<UAction_DataAsset> AXYJ_GameMode::GetEffectivePawnDataSoft() const
{
	if (auto WorldSetting = Cast<AXYJWorldSetting>(GetWorldSettings()))
	{
		return WorldSetting->PawnData;
	}
	return DefaultPawnData;
}

UClass* AXYJ_GameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (auto Data = GetEffectivePawnData())
	{
		return Data->ActorClass;
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AXYJ_GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	PawnFinish.AddDynamic(this, &ThisClass::HandPawnFinish);
}

void AXYJ_GameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AXYJ_GameMode::HandPawnFinish(APawn* StartPawn)
{
	if (auto ActionData = GetEffectivePawnData())
	{
		FOnActionCompled OnActionCompled;
		OnActionCompled.BindDynamic(this,&ThisClass::HandFinish);
		UCancerActionLibrary::LoadActionData(GetEffectivePawnDataSoft(), StartPawn, OnActionCompled);
	}
}

void AXYJ_GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AXYJ_GameMode::HandFinish_Implementation(AActor* Actor)
{
}
