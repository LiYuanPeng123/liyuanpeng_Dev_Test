#pragma once

#include "CoreMinimal.h"
#include "CancerInputBufferComponent.h"
#include "AbilitySystemInterface.h"
#include "Types/FInputTriggerData.h"
#include "Types/FProcessedBinding.h"
#include "Types/FProcessedInputSetup.h"
#include "CancerInputManagerComponent.generated.h"


class UInputMappingContext;
struct FGameplayTag;
class UArrowComponent;
class UCancerInputConfig;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputTriggerTag, const FGameplayTagContainer&, InputTagContainer);

UCLASS(ClassGroup=(CancerInput), BlueprintType, meta=(BlueprintSpawnableComponent))
class CANCERINPUT_API UCancerInputManagerComponent : public UCancerInputBufferComponent, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	static FName ForwardReferenceTag;

	UPROPERTY()
	FOnInputTriggerTag OnReleased;

	UPROPERTY()
	FOnInputTriggerTag OnPressed;

	UPROPERTY()
	FOnInputTriggerTag OnTriggered;

	UPROPERTY(BlueprintReadWrite)
	TMap<FGuid,FInputTriggerData> TriggerDatas;

	UCancerInputManagerComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	UFUNCTION(BlueprintPure, Category = "Cancer Input|Input Manager Component")
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category = "Cancer Input|Input Manager Component")
	AController* GetController() const;

	UFUNCTION(BlueprintPure, Category = "Cancer Input|Manager Component")
	bool IsLocallyControlled() const;

	UFUNCTION(BlueprintPure, Category = "Cancer Input|Input Manager Component")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure, Category = "Cancer Input|Manager Component")
	FVector GetLastInputVector() const;

	UFUNCTION(BlueprintPure, Category = "Cancer Input|Manager Component")
	static UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem(const AController* Controller);

	UFUNCTION(BlueprintPure, Category = "Cancer Input|Input Manager Component")
	bool HasSetupData(const UCancerInputConfig* SetupData) const;

	UFUNCTION(BlueprintPure, Category = "Cancer Input|Input Manager Component")
	bool HasInputMappingContext(const UInputMappingContext* InputMappingContext) const;

	UFUNCTION(BlueprintPure, Category = "Cancer Input|Input Manager Component")
	bool HasCompatibleInputHandler(const UInputAction* InputAction, const ETriggerEvent& TriggerEvent) const;

    UFUNCTION(BlueprintCallable, Category = "Cancer Input|Input Manager Component")
    void AddInputSetupData(const UCancerInputConfig* SetupData);

    UFUNCTION(BlueprintCallable, Category = "Cancer Input|Input Manager Component")
    void RemoveInputSetupData(const UCancerInputConfig* SetupData);
	
    FInputTriggerHandle AddTriggerHandle(const UInputAction* Action, ETriggerEvent Event, int32 InPriority,
                                         UObject* SourceObject, const FOnInputTrigger& Delegate);
	
    bool RemoveTriggerByHandle(const FInputTriggerHandle& Handle);
	

	UFUNCTION(BlueprintCallable, Category = "Cancer Input|Input Manager Component")
	int32 SendGameplayEventToOwner(const FGameplayTag& GameplayEventTag, const FInputActionValue& Value,
	                               const UInputAction* InputAction, bool bSendLocally = true) const;


	void AbilityInputTagReleased(const FGameplayTagContainer& InputTagContainer) const;

	void AbilityInputTagPressed(const FGameplayTagContainer& InputTagContainer) const;

	void AbilityInputTagTriggered(const FGameplayTagContainer& InputTagContainer) const;

	bool bShouldProcessInputs = true;

	UFUNCTION(BlueprintCallable, Category = "Cancer Input|Input Manager Component", DisplayName="启用输入")
	void EnableInputProcessing();
	UFUNCTION(BlueprintCallable, Category = "Cancer Input|Input Manager Component", DisplayName="禁用输入")
	void DisableInputProcessing();
	UFUNCTION(BlueprintCallable, Category = "Cancer Input|Input Manager Component", DisplayName="获取输入状态")
	bool ShouldProcessInputs() { return bShouldProcessInputs; }

protected:
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> InputComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<TObjectPtr<UCancerInputConfig>> InputHandlerSetup;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bAutoClearOnUnPosses;
	
	void InitializeController(AController* NewController);

	void SetupInputComponent(UInputComponent* NewInputComponent);
	
	void AddCachedInputSetupData();
	
	void AddInputMappingContext(UInputMappingContext* InputMappingContext, int32 Priority,
	                            TArray<FProcessedBinding>& OutBindings);
	
	UFUNCTION()
	void DispatchStartedEvent(const FInputActionInstance& ActionInstance);
	
	UFUNCTION()
	void DispatchTriggeredEvent(const FInputActionInstance& ActionInstance);
	
	UFUNCTION()
	void DispatchOngoingEvent(const FInputActionInstance& ActionInstance);
	
	UFUNCTION()
	void DispatchCompletedEvent(const FInputActionInstance& ActionInstance);
	
	UFUNCTION()
	void DispatchCancelledEvent(const FInputActionInstance& ActionInstance);
	
	void Dispatch(const FInputActionInstance& ActionInstance, ETriggerEvent ActualTrigger);
	
	void RemoveInputMappingContext(const UInputMappingContext* InputMappingContext);
	
	virtual void ClearInputSetup();
	
	UFUNCTION()
	void OnPawnRestarted(APawn* Pawn);
	
	UFUNCTION()
	void OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);
	
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Input Manager Component")
	void GetVectorForAxis(const EAxis::Type Axis, FVector& OutReference) const;

private:
	bool bBoundToController;
	
	UPROPERTY()
	TObjectPtr<AController> OwnerController;
	
	UPROPERTY()
	TObjectPtr<UArrowComponent> ForwardReference;
	
	UPROPERTY()
	TMap<TObjectPtr<UInputMappingContext>, FProcessedInputSetup> ProcessedSetups;
	
    UPROPERTY()
    TArray<TObjectPtr<const UCancerInputConfig>> PendingHandlerSetups;
};
