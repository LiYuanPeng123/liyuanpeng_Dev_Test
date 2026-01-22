#include "Components/CancerInputManagerComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerInputHandler.h"
#include "CancerInputLog.h"
#include "CancerInputSettings.h"
#include "CancerInputValuePayload.h"
#include "EnhancedInputSubsystems.h"
#include "FCancerInputHandlerHelpers.h"
#include "InputMappingContext.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Types/FProcessedInputSetup.h"
#include "Data/CancerInputConfig.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "InputHandlers/InputHandler_InputAbility.h"


FName UCancerInputManagerComponent::ForwardReferenceTag = TEXT("InputForwardReference");

UCancerInputManagerComponent::UCancerInputManagerComponent()
{
	bBoundToController = false;
	bAutoClearOnUnPosses = false;
}

void UCancerInputManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	TObjectPtr<APawn> OwnerPawn = nullptr;
	TObjectPtr<APlayerController> PlayerController = nullptr;

	if (GetOwner()->IsA(APawn::StaticClass()))
	{
		OwnerPawn = Cast<APawn>(GetOwner());
		PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
	}
	else if (GetOwner()->IsA(APlayerController::StaticClass()))
	{
		PlayerController = Cast<APlayerController>(GetOwner());
		OwnerPawn = PlayerController->GetPawn();
	}

	if (IsValid(OwnerPawn))
	{
		OwnerPawn->ReceiveRestartedDelegate.AddDynamic(this, &ThisClass::OnPawnRestarted);
		OwnerPawn->ReceiveControllerChangedDelegate.AddDynamic(this, &ThisClass::OnControllerChanged);
		OnPawnRestarted(OwnerPawn);
	}

	InitializeController(PlayerController);
}

void UCancerInputManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const TObjectPtr<UWorld> World = GetWorld();
	if (IsValid(World) && World->IsGameWorld())
	{
		APawn* OwnerPawn = GetPawn();
		if (IsValid(OwnerPawn))
		{
			OwnerPawn->ReceiveRestartedDelegate.RemoveAll(this);
			OwnerPawn->ReceiveControllerChangedDelegate.RemoveAll(this);
		}

		if (IsValid(OwnerController))
		{
			OwnerController->OnPossessedPawnChanged.RemoveAll(this);
			OwnerController = nullptr;
			bBoundToController = false;
		}
	}

	InputComponent = nullptr;
	PendingHandlerSetups.Empty();
	ClearInputSetup();

	Super::EndPlay(EndPlayReason);
}


UEnhancedInputLocalPlayerSubsystem* UCancerInputManagerComponent::GetEnhancedInputSubsystem(
	const AController* Controller)
{
	if (IsValid(Controller))
	{
		const APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (IsValid(PlayerController))
		{
			const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
			if (IsValid(LocalPlayer))
			{
				return LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			}
		}
	}

	return nullptr;
}

bool UCancerInputManagerComponent::HasSetupData(const UCancerInputConfig* SetupData) const
{
	for (auto It(ProcessedSetups.CreateConstIterator()); It; ++It)
	{
		if (It.Value().SourceData == SetupData)
		{
			return true;
		}
	}

	return false;
}

bool UCancerInputManagerComponent::HasInputMappingContext(const UInputMappingContext* InputMappingContext) const
{
	check(IsValid(InputMappingContext));

	const UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputSubsystem(OwnerController.Get());
	return IsValid(Subsystem) && Subsystem->HasMappingContext(InputMappingContext);
}

bool UCancerInputManagerComponent::HasCompatibleInputHandler(const UInputAction* InputAction,
                                                             const ETriggerEvent& TriggerEvent) const
{
	for (auto It(ProcessedSetups.CreateConstIterator()); It; ++It)
	{
		if (It.Value().SourceData->HasCompatibleInputHandler(InputAction, TriggerEvent))
		{
			return true;
		}
	}

	return false;
}

void UCancerInputManagerComponent::AddInputSetupData(const UCancerInputConfig* SetupData)
{
	check(SetupData);

	// If not fully initialized yet, then cache this for later.
	if (!bBoundToController)
	{
		PendingHandlerSetups.AddUnique(SetupData);
		return;
	}

	// Make sure that this setup has not been already processed.
	if (HasSetupData(SetupData))
	{
		UE_LOG(CancerInputLog, Warning, TEXT("[%s] Received a duplicated Input Setup (%s)!"),
		       *GetNameSafe(GetOwner()), *GetNameSafe(SetupData));

		return;
	}
	SetupData->InitInput();
	// Also make sure we don't have the same IMC from another Setup.
	const TObjectPtr<UInputMappingContext> NewContext = SetupData->InputMappingContext;
	if (HasInputMappingContext(NewContext))
	{
		if (ProcessedSetups.Contains(NewContext))
		{
			const FProcessedInputSetup OriginalSetup = *ProcessedSetups.Find(NewContext);
			UE_LOG(CancerInputLog, Warning, TEXT("[%s] Duplicated IMC '%s' from setup '%s'! Original setup: '%s'."),
			       *GetNameSafe(GetOwner()), *GetNameSafe(NewContext), *GetNameSafe(SetupData),
			       *GetNameSafe(OriginalSetup.SourceData));
		}
		else
		{
			UE_LOG(CancerInputLog, Warning,
			       TEXT("[%s] Duplicated IMC '%s' from setup '%s'! No original setup, maybe from a previous pawn?"),
			       *GetNameSafe(GetOwner()), *GetNameSafe(NewContext), *GetNameSafe(SetupData));
		}

		return;
	}

	TArray<FProcessedBinding> Bindings;
	AddInputMappingContext(NewContext, SetupData->Priority, Bindings);

	if (!Bindings.IsEmpty())
	{
		const FProcessedInputSetup Setup(SetupData, Bindings);
		ProcessedSetups.Add(NewContext, Setup);

		UE_LOG(CancerInputLog, Log, TEXT("[%s] Added Setup Data %s with %d bindings."),
		       *GetNameSafe(GetOwner()), *GetNameSafe(SetupData), Bindings.Num());
	}
	else
	{
		// We don't have any bindings so this Context is pointless. Remove it so it can be added again later.
		RemoveInputMappingContext(NewContext);

		UE_LOG(CancerInputLog, Warning, TEXT("[%s] Discarded Setup Data %s as it has no bindings."),
		       *GetNameSafe(GetOwner()), *GetNameSafe(SetupData));
	}
}

void UCancerInputManagerComponent::RemoveInputSetupData(const UCancerInputConfig* SetupData)
{
	if (IsValid(SetupData))
	{
		// Remove buffered commands to avoid them triggering later, to a wrong pawn.
		Execute_ResetInputBuffer(this);

		// Remove the mapping context related to this setup.
		RemoveInputMappingContext(SetupData->InputMappingContext);
	}
}

FInputTriggerHandle UCancerInputManagerComponent::AddTriggerHandle(const UInputAction* Action, ETriggerEvent Event,
                                                                   int32 InPriority, UObject* SourceObject,
                                                                   const FOnInputTrigger& Delegate)
{
	FInputTriggerHandle Handle;
	if (!Action || !SourceObject)
	{
		return Handle;
	}
	FInputTriggerData NewData;
	NewData.TriggerAction = const_cast<UInputAction*>(Action);
	NewData.TriggerEvent = Event;
	NewData.Priority = InPriority;
	NewData.OnTriggerEvent.AddUnique(Delegate);
	NewData.Source = SourceObject;
	Handle.Id = FGuid::NewGuid();
	TriggerDatas.Add(Handle.Id, NewData);
	return Handle;
}

bool UCancerInputManagerComponent::RemoveTriggerByHandle(const FInputTriggerHandle& Handle)
{
	if (FInputTriggerData* Trigger = TriggerDatas.Find(Handle.Id))
	{
		Trigger->RemoveBindEvent();
		TriggerDatas.Remove(Handle.Id);
		return true;
	}
	return false;
}


void UCancerInputManagerComponent::InitializeController(AController* NewController)
{
	if (IsValid(OwnerController) && OwnerController == GetOwner())
	{
		// Owned by a controller and it's already initialized.
		return;
	}

	if (IsValid(OwnerController) && GetOwner() != OwnerController)
	{
		if (bAutoClearOnUnPosses)
		{
			ClearInputSetup();
		}

		OwnerController->OnPossessedPawnChanged.RemoveAll(this);
		OwnerController = nullptr;
		bBoundToController = false;
	}

	if (IsValid(NewController))
	{
		OwnerController = NewController;
		OwnerController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
		bBoundToController = true;

		if (IsValid(OwnerController->InputComponent))
		{
			SetupInputComponent(OwnerController->InputComponent);
			AddCachedInputSetupData();
		}
	}
}

void UCancerInputManagerComponent::SetupInputComponent(UInputComponent* NewInputComponent)
{
	InputComponent = Cast<UEnhancedInputComponent>(NewInputComponent);
	if (ensureMsgf(InputComponent, TEXT("Please make sure to use a valid EnhancedInputComponent!")))
	{
		if (IsValid(OwnerController))
		{
			for (const TObjectPtr<const UCancerInputConfig> SetupData : InputHandlerSetup)
			{
				AddInputSetupData(SetupData);
			}
		}
	}
}

void UCancerInputManagerComponent::AddCachedInputSetupData()
{
	for (auto It = PendingHandlerSetups.CreateIterator(); It; ++It)
	{
		const UCancerInputConfig* PendingSetup = *It;
		AddInputSetupData(PendingSetup);
		It.RemoveCurrent();
	}
}

void UCancerInputManagerComponent::AddInputMappingContext(UInputMappingContext* InputMappingContext,
                                                          int32 Priority, TArray<FProcessedBinding>& OutBindings)
{
	OutBindings.Reset();

	if (IsValid(InputComponent) && ensure(IsValid(InputMappingContext)))
	{
		if (IsValid(InputMappingContext) && !HasInputMappingContext(InputMappingContext))
		{
			const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = GetEnhancedInputSubsystem(
				OwnerController.Get());
			check(IsValid(Subsystem));

			if (UCancerInputSubsystem* CancerInputSubsystem = UCancerInputSubsystem::GetCancerInputSubsystem(
				OwnerController))
			{
				CancerInputSubsystem->ActivateInputMappingContext(FInputActionMappingData(InputMappingContext,
					UCancerInputSettings::Get()->DefaultGameInputTag));
			}

			//Subsystem->AddMappingContext(InputMappingContext, Priority);

			TArray<TObjectPtr<const UInputAction>> ActionsToBind;
			ActionsToBind.Reserve(InputMappingContext->GetMappings().Num());
			OutBindings.Reserve(ActionsToBind.Num());

			// Ensure that we only process each action once, regardless of how many keys are assigned to them.
			for (const auto& KeyMapping : InputMappingContext->GetMappings())
			{
				ActionsToBind.AddUnique(KeyMapping.Action);
			}

			// Create bindings for each unique action, for every tracked trigger and then store the Handle.
			for (const auto InputAction : ActionsToBind)
			{
				const TArray<ETriggerEvent> TrackedEvents = GetDefault<UCancerInputSettings>()->TrackedEvents;
				for (const ETriggerEvent TriggerEvent : TrackedEvents)
				{
					FEnhancedInputActionHandlerInstanceSignature::TMethodPtr<ThisClass> DispatchFunction = nullptr;
					switch (TriggerEvent)
					{
					case ETriggerEvent::Started:
						DispatchFunction = &ThisClass::DispatchStartedEvent;
						break;
					case ETriggerEvent::Triggered:
						DispatchFunction = &ThisClass::DispatchTriggeredEvent;
						break;
					case ETriggerEvent::Ongoing:
						DispatchFunction = &ThisClass::DispatchOngoingEvent;
						break;
					case ETriggerEvent::Completed:
						DispatchFunction = &ThisClass::DispatchCompletedEvent;
						break;
					case ETriggerEvent::Canceled:
						DispatchFunction = &ThisClass::DispatchCancelledEvent;
						break;
					default:
						const UEnum* EnumPtr = StaticEnum<ETriggerEvent>();
						UE_LOG(CancerInputLog, Warning, TEXT("[%s] Unable to handle Trigger Event '%s'."),
						       *GetNameSafe(GetOwner()), *EnumPtr->GetValueAsString(TriggerEvent));
					}

					if (DispatchFunction != nullptr)
					{
						FEnhancedInputActionEventBinding* Handle = &InputComponent->BindAction(
							InputAction, TriggerEvent, this, DispatchFunction);
						if (Handle != nullptr)
						{
							const FProcessedBinding& Binding = FProcessedBinding(InputAction, TriggerEvent, Handle);
							OutBindings.Add(Binding);
						}
					}
				}
			}
		}
	}
}

void UCancerInputManagerComponent::DispatchStartedEvent(const FInputActionInstance& ActionInstance)
{
	Dispatch(ActionInstance, ETriggerEvent::Started);
}

void UCancerInputManagerComponent::DispatchTriggeredEvent(const FInputActionInstance& ActionInstance)
{
	Dispatch(ActionInstance, ETriggerEvent::Triggered);
}

void UCancerInputManagerComponent::DispatchOngoingEvent(const FInputActionInstance& ActionInstance)
{
	Dispatch(ActionInstance, ETriggerEvent::Ongoing);
}

void UCancerInputManagerComponent::DispatchCompletedEvent(const FInputActionInstance& ActionInstance)
{
	Dispatch(ActionInstance, ETriggerEvent::Completed);
}

void UCancerInputManagerComponent::DispatchCancelledEvent(const FInputActionInstance& ActionInstance)
{
	Dispatch(ActionInstance, ETriggerEvent::Canceled);
}

void UCancerInputManagerComponent::Dispatch(const FInputActionInstance& ActionInstance,
                                            ETriggerEvent ActualTrigger)
{
	const TObjectPtr<const UInputAction> InputAction = ActionInstance.GetSourceAction();
	if (!ShouldProcessInputs())
	{
		return;
	}
	TArray<FBufferedInputCommand> CandidateCommands;
	const TObjectPtr<UActorComponent> InputBuffer = GetInputBufferComponent();
	for (auto It(ProcessedSetups.CreateConstIterator()); It; ++It)
	{
		auto Handler = ProcessedSetups.Find(It.Key())->SourceData->InputHandler;
		if (IsValid(Handler) && Handler->CanHandle(ActualTrigger, InputAction))
		{
			const FGameplayTag BufferChannelTag = Handler->GetBufferChannelTag();
			const bool bIsUsingBuffer = IsValid(InputBuffer) &&
				Execute_IsInputBufferOpen(InputBuffer, BufferChannelTag);
			if (bIsUsingBuffer && Handler->CanBeBuffered(InputAction.Get()) && ActualTrigger == ETriggerEvent::Started)
			{
				UE_LOG(CancerInputLog, Warning, TEXT("输入缓存:[%s] [%s][%s]"), *GetNameSafe(GetOwner()),
				       *GetNameSafe(InputAction),
				       *StaticEnum<ETriggerEvent>()->GetNameStringByValue(static_cast<int64>(ActualTrigger)));
				FBufferedInputCommand NewCommand(this, Handler, ActionInstance, ActualTrigger);
				CandidateCommands.AddUnique(NewCommand);
			}
			else
			{
				//UE_LOG(CancerInputLog, Warning, TEXT("输入:[%s] %s  %s."), *GetNameSafe(GetOwner()),
				//       *GetNameSafe(InputAction), *GetNameSafe(Handler));
				Handler->SetWorld(GetWorld());
				Handler->HandleInput(this, ActionInstance, ActualTrigger);
			}

			TArray<FInputTriggerData> ExecuteTriggers;
			for (auto Pair : TriggerDatas)
			{
				if (Pair.Value.TriggerAction == InputAction && Pair.Value.TriggerEvent == ActualTrigger)
				{
					ExecuteTriggers.Add(Pair.Value);
				}
			}
			ExecuteTriggers.Sort([](const FInputTriggerData& A, const FInputTriggerData& B)
			{
				return A.Priority < B.Priority;
			});
			auto InputData = NewObject<UCancerInputValuePayload>(GetOwner());
			FGameplayEventData EventData;
			EventData.Instigator = GetOwner();
			InputData->InputActionIns = ActionInstance;
			EventData.OptionalObject = InputData;
			EventData.EventMagnitude = ActionInstance.GetTriggeredTime();
			for (auto Trigger : ExecuteTriggers)
			{
				Trigger.OnTriggerEvent.Broadcast(EventData);
			}
		}
	}
	if (CandidateCommands.Num() > 0)
	{
		Execute_BufferInputCommands(InputBuffer, CandidateCommands);
	}
}

void UCancerInputManagerComponent::RemoveInputMappingContext(const UInputMappingContext* InputMappingContext)
{
	if (ensure(IsValid(InputMappingContext)) && HasInputMappingContext(InputMappingContext))
	{
		if (ProcessedSetups.Contains(InputMappingContext))
		{
			FProcessedInputSetup Setup = *ProcessedSetups.Find(InputMappingContext);
			for (auto It = Setup.ProcessedBindings.CreateIterator(); It; ++It)
			{
				// Deliberately not using "IsValid()", as it may have GC flags if we're in the "EndPlay" flow.
				if (InputComponent)
				{
					const FProcessedBinding Binding = *It;
					if (!InputComponent->RemoveBinding(*Binding.Handle))
					{
						UE_LOG(CancerInputLog, Warning, TEXT("[%s] Unable to remove binding for %s."),
						       *GetNameSafe(GetOwner()), *GetNameSafe(Binding.InputAction));
					}
				}

				// We still want to remove this entry, regardless of the Input Component being available or not.
				It.RemoveCurrent();
			}
		}

		const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = GetEnhancedInputSubsystem(
			OwnerController.Get());
		check(IsValid(Subsystem));
		Subsystem->RemoveMappingContext(InputMappingContext);
		ProcessedSetups.Remove(InputMappingContext);

		UE_LOG(CancerInputLog, Log, TEXT("[%s] Removed Input Context %s."),
		       *GetNameSafe(GetOwner()), *GetNameSafe(InputMappingContext));
	}
}

void UCancerInputManagerComponent::ClearInputSetup()
{
	for (auto It = ProcessedSetups.CreateIterator(); It; ++It)
	{
		const UCancerInputConfig* Data = It->Value.SourceData;
		RemoveInputSetupData(Data);
	}
}

void UCancerInputManagerComponent::OnPawnRestarted(APawn* Pawn)
{
	if (IsValid(Pawn))
	{
		ForwardReference = Pawn->FindComponentByTag<UArrowComponent>(ForwardReferenceTag);
		if (ForwardReference)
		{
			UE_LOG(CancerInputLog, Log, TEXT("[%s] Retrieved a new Forward Reference %s."),
			       *GetNameSafe(GetOwner()), *GetNameSafe(ForwardReference));
		}

		/*if (Pawn->Implements<UInputSetupProviderInterface>())
		{
			TArray<UNinjaInputSetupDataAsset*> SetupsToAdd = IInputSetupProviderInterface::Execute_GetAddedSetups(Pawn);
			for (const UNinjaInputSetupDataAsset* Setup : SetupsToAdd)
			{
				AddInputSetupData(Setup);
			}
		}*/
	}
	else
	{
		ForwardReference = nullptr;
	}
}

void UCancerInputManagerComponent::OnControllerChanged(APawn* Pawn, AController* OldController,
                                                       AController* NewController)
{
	InitializeController(NewController);
}

void UCancerInputManagerComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (bAutoClearOnUnPosses)
	{
		ClearInputSetup();
	}
	/*else if (OldPawn && OldPawn->Implements<UInputSetupProviderInterface>())
	{
		TArray<UNinjaInputSetupDataAsset*> SetupsToRemove = IInputSetupProviderInterface::Execute_GetRemovedSetups(OldPawn);
		for (const UNinjaInputSetupDataAsset* Setup : SetupsToRemove)
		{
			RemoveInputSetupData(Setup);
		}
	}*/

	OnPawnRestarted(NewPawn);
}

void UCancerInputManagerComponent::GetVectorForAxis_Implementation(const EAxis::Type Axis, FVector& OutReference) const
{
	if (IsValid(ForwardReference))
	{
		const FRotator ReferenceYawRotation = ForwardReference->GetComponentRotation();
		OutReference = FRotationMatrix(ReferenceYawRotation).GetScaledAxis(Axis);
	}
	else if (IsValid(OwnerController))
	{
		FRotator ControlRotation = OwnerController->GetControlRotation();
		ControlRotation.Roll = 0.f;
		ControlRotation.Pitch = 0.f;
		OutReference = FRotationMatrix(ControlRotation).GetScaledAxis(Axis);
	}
	else
		switch (Axis)
		{
		case EAxis::X:
			OutReference = GetPawn()->GetActorForwardVector();
			break;
		case EAxis::Y:
			OutReference = GetPawn()->GetActorRightVector();
			break;
		case EAxis::Z:
			OutReference = GetPawn()->GetActorUpVector();
			break;
		default:
			checkNoEntry();
		}
}

FVector UCancerInputManagerComponent::GetLastInputVector() const
{
	const TObjectPtr<APawn> PawnOwner = GetPawn();
	if (IsValid(PawnOwner))
	{
		/*if (PawnOwner->Implements<ULastInputProviderInterface>())
		{
			return ILastInputProviderInterface::Execute_GetLastInputVector(PawnOwner);
		}*/

		const TObjectPtr<UPawnMovementComponent> PawnMovement = PawnOwner->GetMovementComponent();
		if (IsValid(PawnOwner->GetMovementComponent()))
		{
			return PawnMovement->GetLastInputVector();
		}
	}

	return FVector::ZeroVector;
}


UAbilitySystemComponent* UCancerInputManagerComponent::GetAbilitySystemComponent() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn());
}


int32 UCancerInputManagerComponent::SendGameplayEventToOwner(const FGameplayTag& GameplayEventTag,
                                                             const FInputActionValue& Value,
                                                             const UInputAction* InputAction, bool bSendLocally) const
{
	int32 Activations = 0;
	const TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetController());

	if (IsValid(PlayerController) && ensureMsgf(GameplayEventTag.IsValid(),
	                                            TEXT("The Gameplay Event Tag must be valid.")))
	{
		if ((bSendLocally && PlayerController->IsLocalController()))
		{
			// Local execution for either local client or authoritative version.
			Activations = FCancerInputHandlerHelpers::SendGameplayEvent(this, GameplayEventTag, Value, InputAction);
		}
	}

	return Activations;
}

void UCancerInputManagerComponent::AbilityInputTagReleased(const FGameplayTagContainer& InputTagContainer) const
{
	OnReleased.Broadcast(InputTagContainer);
}

void UCancerInputManagerComponent::AbilityInputTagPressed(const FGameplayTagContainer& InputTagContainer) const
{
	OnPressed.Broadcast(InputTagContainer);
}

void UCancerInputManagerComponent::AbilityInputTagTriggered(const FGameplayTagContainer& InputTagContainer) const
{
	OnTriggered.Broadcast(InputTagContainer);
}

void UCancerInputManagerComponent::EnableInputProcessing()
{
	bShouldProcessInputs = true;
}

void UCancerInputManagerComponent::DisableInputProcessing()
{
	bShouldProcessInputs = false;
}


APawn* UCancerInputManagerComponent::GetPawn() const
{
	if (GetOwner()->IsA(APawn::StaticClass()))
	{
		return Cast<APawn>(GetOwner());
	}

	if (GetOwner()->IsA(AController::StaticClass()))
	{
		const AController* Controller = Cast<AController>(GetOwner());
		return Controller->GetPawn();
	}

	return nullptr;
}

AController* UCancerInputManagerComponent::GetController() const
{
	return OwnerController;
}

bool UCancerInputManagerComponent::IsLocallyControlled() const
{
	return IsValid(OwnerController) && OwnerController->IsLocalController();
}
