#include "XYJ_GameMode.h"
#include "CancerAssetManager.h"
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

UClass* AXYJ_GameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (auto Data = GetEffectivePawnData())
	{
		if (!Data->PawnClass.IsNull())
		{
			return UCancerAssetManager::Get().GetSubclass(Data->PawnClass);
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}


void AXYJ_GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	PawnFinish.AddDynamic(this, &ThisClass::HandPawnFinish);
	/*UWorld* World = GetWorld();
	if (!World) return;
	TArray<AActor*> Creators;
	UGameplayStatics::GetAllActorsOfClass(World, AAICreator::StaticClass(), Creators);
	for (AActor* Creator : Creators)
	{
		if (auto AICreator = Cast<AAICreator>(Creator))
		{
			APawn* NewPawn = nullptr;
			TSharedPtr<FStreamableHandle> Handle = AICreator->CreateCustomPlayerPawn(NewPawn);
			if (NewPawn && Handle.IsValid())
			{
				PawnLoadHandles.Add(NewPawn, Handle);
			}
		}
	}*/
}

void AXYJ_GameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AXYJ_GameMode::HandPawnFinish(APawn* StartPawn)
{
	if (auto ActionData = GetEffectivePawnData())
	{
		TSharedPtr<FStreamableHandle> Handle = ActionData->StartAsyncLoadAndExecuteWithHandle(
			StartPawn, FSimpleDelegate::CreateLambda(
				[this, StartPawn, ActionData]()
				{
					ActionData->PreInitComponent();
					ActionData->PostInitComponent();
					ActionData->FinalizeAfterComponent();
					if (AXYJ_Character* Character = Cast<AXYJ_Character>(StartPawn))
					{
						if (auto MovementComponent = Character->GetCharacterMovement())
							Character->OnMovementModeChanged(
								MovementComponent->MovementMode,
								MovementComponent->CustomMovementMode);
						if (Character->Implements<UCancerActionInterface>())
						{
							ICancerActionInterface::Execute_HandleOnActionCompleted(Character);
						}
					}
					if (StartPawn)
					{
						UAIPerceptionStimuliSourceComponent* Stimuli = StartPawn->FindComponentByClass<UAIPerceptionStimuliSourceComponent>();
						if (!Stimuli)
						{
							Stimuli = NewObject<UAIPerceptionStimuliSourceComponent>(StartPawn);
							Stimuli->RegisterComponent();
						}
						if (Stimuli)
						{
							Stimuli->RegisterForSense(UAISense_Sight::StaticClass());
						}
					}
					PawnLoadHandles.Remove(StartPawn);
				}));
		if (Handle.IsValid())
		{
			PawnLoadHandles.Add(StartPawn, Handle);
		}
	}
}


float AXYJ_GameMode::GetWorldLoadProgress() const
{
	if (PawnLoadHandles.Num() == 0) return 1.f;
	double Sum = 0.0;
	int32 Count = 0;
	for (const auto& Pair : PawnLoadHandles)
	{
		if (Pair.Key.IsValid() && Pair.Value.IsValid())
		{
			Sum += Pair.Value->GetProgress();
			++Count;
		}
	}
	return Count > 0 ? static_cast<float>(Sum / Count) : 1.f;
}

void AXYJ_GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const float Progress = GetWorldLoadProgress();
	if (!FMath::IsNearlyEqual(Progress, LastProgress))
	{
		OnPawnLoadProgress.Broadcast(Progress);
		LastProgress = Progress;
	}
	if (!bLoadCompleted && Progress >= 1.0f)
	{
		OnPawnLoadProgress.Broadcast(1.0f);
		bLoadCompleted = true;
		SetActorTickEnabled(false);
	}
}
