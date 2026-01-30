#include "CancerGameMode.h"
#include "CancerCharacter.h"
#include "CancerPlayerController.h"
#include "CancerPlayerStart.h"
#include "EngineUtils.h"


DEFINE_LOG_CATEGORY_STATIC(LogCancer, Log, All);

ACancerGameMode::ACancerGameMode(const FObjectInitializer& ObjectInitializer)
{
	PlayerControllerClass = ACancerPlayerController::StaticClass();
	DefaultPawnClass = ACancerCharacter::StaticClass();
	CachePlayerStart = nullptr;
}

void ACancerGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	for (TActorIterator<ACancerPlayerStart> It(GetWorld()); It; ++It)
	{
		if (ACancerPlayerStart* PlayerStart = *It)
		{
			CachePlayerStart = PlayerStart;
		}
	}
}

void ACancerGameMode::StartPlay()
{
	Super::StartPlay();
}

bool ACancerGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	//使用自定义的位置生成
	return false;
}

UClass* ACancerGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	//此处返回我们重载的PawnClass
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

bool ACancerGameMode::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	return true;
}

APawn* ACancerGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
                                                                   const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			SpawnedPawn->FinishSpawning(SpawnTransform);

			PawnFinish.Broadcast(SpawnedPawn);
			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogCancer, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."),
			       *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogCancer, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

AActor* ACancerGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	return CachePlayerStart ? CachePlayerStart : Super::ChoosePlayerStart_Implementation(Player);
}

void ACancerGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
}
