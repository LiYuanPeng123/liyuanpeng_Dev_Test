#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CancerGameMode.generated.h"

class ACancerPlayerStart;


/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnFinish, APawn*, Pawn);

UCLASS()
class CANCERGAMEPLAY_API ACancerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPawnFinish PawnFinish;

	ACancerGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;

	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;
	virtual APawn*
	SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	// Restart (respawn) the specified player or bot next frame
	// - If bForceReset is true, the controller will be reset this frame (abandoning the currently possessed pawn, if any)
	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ACancerPlayerStart* CachePlayerStart; //缓存玩家生成点
};
