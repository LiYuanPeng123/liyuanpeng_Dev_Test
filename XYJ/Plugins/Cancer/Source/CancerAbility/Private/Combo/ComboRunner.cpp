#include "Combo/ComboRunner.h"
#include "Combo/ComboController.h"
#include "Combo/ComboGraph.h"
#include "Combo/ComboInput.h"
#include "Combo/ComboMontagePlayer.h"
#include "Combo/MoveDefinition.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
 
static void BufferAndAdvance(class UComboController* Controller, const FGameplayTagContainer& Tags)
{
	if (Controller)
	{
		Controller->BufferInput(Tags);
		Controller->TryAdvance();
	}
}

UComboRunner::UComboRunner()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UComboRunner::BeginPlay()
{
	Super::BeginPlay();
	Controller = GetOwner() ? GetOwner()->FindComponentByClass<UComboController>() : nullptr;
	Input = GetOwner() ? GetOwner()->FindComponentByClass<UComboInputComponent>() : nullptr;
	Player = GetOwner() ? GetOwner()->FindComponentByClass<UComboMontagePlayer>() : nullptr;

	if (!Controller)
	{
		Controller = NewObject<UComboController>(GetOwner());
		Controller->RegisterComponent();
	}
	if (!Input)
	{
		Input = NewObject<UComboInputComponent>(GetOwner());
		Input->RegisterComponent();
	}
	if (!Player)
	{
		Player = NewObject<UComboMontagePlayer>(GetOwner());
		Player->RegisterComponent();
	}

	if (Controller)
	{
		Controller->OnMoveStarted.AddDynamic(this, &UComboRunner::HandleMoveStarted);
		Controller->OnMoveEnded.AddDynamic(this, &UComboRunner::HandleMoveEnded);
	}
	if (Input)
	{
		Input->OnInputResolved.AddDynamic(this, &UComboRunner::HandleInputResolved);
	}
}

void UComboRunner::Initialize(UComboGraph* InGraph)
{
	Graph = InGraph;
	if (Controller && Graph)
	{
		Controller->InitializeGraph(Graph);
	}
}

void UComboRunner::Start()
{
	if (Controller)
	{
		Controller->StartCombo(nullptr);
	}
}

void UComboRunner::FeedInput(const FGameplayTagContainer& InputTags)
{
	if (Input)
	{
		Input->BufferPressTags(InputTags);
	}
	BufferAndAdvance(Controller, InputTags);
}

void UComboRunner::HandleMoveStarted(UMoveDefinition* Move)
{
	if (!Move || !Player) return;
	UAnimMontage* Montage = Move->ResolveMontage();
	if (!Montage) return;
	Player->Play(Montage, Move->SectionName, Move->PlayRate, Move->RootMotionScale);
}

void UComboRunner::HandleMoveEnded(UMoveDefinition* Move)
{
}

void UComboRunner::HandleInputResolved(const FComboResolvedInput& Resolved)
{
	if (Controller)
	{
		// If pre-input window is open and combo window is closed, stage as pre-input.
		// Otherwise, buffer and try advance immediately.
		if (Resolved.Tags.IsEmpty())
		{
			return;
		}
		if (Controller->GetCurrentMove() && !Resolved.Tags.IsEmpty())
		{
			// There is no direct query for pre-input open; controller manages it internally.
			// We'll always buffer; controller will stash it as pre-input if window isn't open.
			Controller->BufferInput(Resolved.Tags);
			Controller->TryAdvance();
		}
	}
}
