#pragma once

#include "CoreMinimal.h"
#include "CancerCameraAssistInterface.h"
#include "GameplayTagContainer.h"
#include "TeamConfigDataAsset.h"
#include "Components/CancerTeamComponent.h"
#include "GameFramework/PlayerController.h"
#include "CancerPlayerController.generated.h"


class UCancerAbilitySystemComponent;

UCLASS()
class CANCERGAMEPLAY_API ACancerPlayerController : public APlayerController, public ICancerCameraAssistInterface

{
	GENERATED_BODY()

public:
	UCancerAbilitySystemComponent* GetCancerAbilitySystemComponent() const;
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	//~APlayerController interface
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void
	UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	//~End of APlayerController interface

protected:
	UPROPERTY(BlueprintReadWrite)
	bool bHideViewTargetPawnNextFrame = false;
};
