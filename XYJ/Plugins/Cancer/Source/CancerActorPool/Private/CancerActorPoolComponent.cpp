#include "CancerActorPoolComponent.h"

#include "Interfaces/PoolableActorInterface.h"

DEFINE_LOG_CATEGORY_STATIC(ActorPool, Log, All);

UCancerActorPoolComponent::UCancerActorPoolComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);

	CollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

void UCancerActorPoolComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UCancerActorPoolComponent::BeginPlay()
{
	Super::BeginPlay();
	Pool.SetPoolManager(this);
	InitializePool();
}

void UCancerActorPoolComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Pool.Empty();
	Super::EndPlay(EndPlayReason);
}

AActor* UCancerActorPoolComponent::GetActorFromPool(TSubclassOf<AActor> ActorClass, bool bActivate)
{
	AActor* Actor = Pool.GetActor(ActorClass);
	if (IsValid(Actor))
	{
		if (bActivate)
		{
			IPoolableActorInterface::Execute_Activate(Actor);
		}
		else
		{
			IPoolableActorInterface::Execute_Reserve(Actor);
		}
	}
	else
	{
		UE_LOG(ActorPool, Warning, TEXT("No actor instance available for class %s."), *GetNameSafe(ActorClass));
	}

	return Actor;
}

AActor* UCancerActorPoolComponent::GetActorByTagFromPool(const FGameplayTag& InTag, bool bActivate)
{
	AActor* Actor = Pool.GetActorByTag(InTag);
	if (IsValid(Actor))
	{
		if (bActivate)
		{
			IPoolableActorInterface::Execute_Activate(Actor);
		}
		else
		{
			IPoolableActorInterface::Execute_Reserve(Actor);
		}
	}
	else
	{
		UE_LOG(ActorPool, Warning, TEXT("No actor instance available for tag %s."), *InTag.ToString());
	}

	return Actor;
}

AActor* UCancerActorPoolComponent::SpawnActor(const TSubclassOf<AActor>& ActorClass, const FGuid& PoolableActorId)
{
	if (!IsValid(ActorClass) || !ActorClass->ImplementsInterface(UPoolableActorInterface::StaticClass()))
	{
		UE_LOG(ActorPool, Warning,
		       TEXT("Provided actor class (%s) is null or does not implement CombatPoolableActorInterface."),
		       *GetNameSafe(ActorClass));
		return nullptr;
	}

	if (!PoolableActorId.IsValid())
	{
		UE_LOG(ActorPool, Warning, TEXT("Invalid Poolable Actor ID."));
		return nullptr;
	}

	const AActor* DefaultActor = GetDefault<AActor>(ActorClass);
	if (DefaultActor->bOnlyRelevantToOwner && !OwnerIsLocallyControlled())
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters = BuildSpawnParameters(ActorClass, PoolableActorId);
	SpawnParameters.bDeferConstruction = true;
	
	SpawnParameters.Owner = GetOwner();

	AActor* NewActor = GetWorld()->SpawnActor<AActor>(ActorClass, SpawnParameters);
	if (IsValid(NewActor))
	{
		NewActor->bNetStartup = true;
		NewActor->bNetLoadOnClient = true;
		NewActor->SetNetAddressable();

		IPoolableActorInterface::Execute_InitializedByPool(NewActor, this);
		InitializeActor(NewActor);

		const FTransform InitialTransform = BuildInitialTransform(NewActor);
		NewActor->FinishSpawning(InitialTransform);

		UE_LOG(ActorPool, Warning, TEXT("Spawned actor %s from class %s."), *GetNameSafe(NewActor),
		       *GetNameSafe(ActorClass));
	}

	return NewActor;
}

void UCancerActorPoolComponent::InitializePool()
{
	for (const FPoolableActorInfo& Info : PoolableActors)
	{
		Pool.SetActorPoolSize(Info.PoolableActorClass, Info.PoolSize);
	}

	Pool.Initialize();
}

void UCancerActorPoolComponent::InitializeActor(AActor* NewActor)
{
	check(IsValid(NewActor));
	NewActor->SetActorEnableCollision(false);
	NewActor->SetActorHiddenInGame(true);
	NewActor->SetActorTickEnabled(false);
}

bool UCancerActorPoolComponent::OwnerIsLocallyControlled() const
{
	const APawn* PawnOwner = Cast<APawn>(GetOwner());
	return PawnOwner && PawnOwner->IsLocallyControlled();
}

APawn* UCancerActorPoolComponent::GetInstigator() const
{
	return Cast<APawn>(GetOwner());
}

FActorSpawnParameters UCancerActorPoolComponent::BuildSpawnParameters(const TSubclassOf<AActor>& ActorClass,
                                                                      const FGuid& PoolableActorId) const
{
	AActor* Owner = GetOwner();
	const FString NewName = BuildActorName(ActorClass, PoolableActorId);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.bDeferConstruction = true;
	SpawnParameters.Name = FName(NewName);
	SpawnParameters.Owner = Owner;
	SpawnParameters.Instigator = GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride = CollisionHandlingMethod;

	return SpawnParameters;
}

FString UCancerActorPoolComponent::BuildActorName(const TSubclassOf<AActor>& ActorClass,
                                                  const FGuid& PoolableActorId) const
{
	return FString::Printf(TEXT("%ls_%ls"), *ActorClass->GetDefaultObjectName().ToString(),
	                       *PoolableActorId.ToString());
}

FTransform UCancerActorPoolComponent::BuildInitialTransform(const AActor* NewActor)
{
	return FTransform::Identity;
}
