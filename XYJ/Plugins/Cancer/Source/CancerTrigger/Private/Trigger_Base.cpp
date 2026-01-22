#include "Trigger_Base.h"

#include "Components/ArrowComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Trigger/TriggerShape_Base.h"

#if WITH_EDITOR
void UItemTrigger_Base::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property)
	{
		return;
	}
	const FName PropName = PropertyChangedEvent.Property->GetFName();
	if (!(PropName == TEXT("TriggerShape") || PropName == TEXT("LoadShape") || PropName == TEXT("bUseLoadShape")))
	{
		return;
	}

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		UWorld* World = WorldContext.World();
		if (!World)
		{
			continue;
		}
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(World, ATrigger_Base::StaticClass(), Actors);
		for (AActor* Actor : Actors)
		{
			ATrigger_Base* Trigger = Cast<ATrigger_Base>(Actor);
			if (Trigger && Trigger->TriggerData == this)
			{
				Trigger->RerunConstructionScripts();
			}
		}
	}
}
#endif


ATrigger_Base::ATrigger_Base(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bGenerateOverlapEventsDuringLevelStreaming = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);
}

void ATrigger_Base::BeginPlay()
{
	Super::BeginPlay();
	if (TriggerData == nullptr) return;
	if (TriggerData->bUseLoadShape)
	{
		if (CollisionLoadComponent == nullptr) return;
	}
	if (CollisionTriggerComponent == nullptr) return;

#if WITH_EDITOR
	if (TriggerData->bUseLoadShape)
	{
	}
#endif

	if (TriggerData->bUseLoadShape)
	{
		CollisionLoadComponent->OnComponentBeginOverlap.AddDynamic(
			this, &ATrigger_Base::BeginLoadComponentOverlap);
		CollisionLoadComponent->OnComponentEndOverlap.AddDynamic(
			this, &ATrigger_Base::EndLoadComponentOverlap);
	}

	CollisionTriggerComponent->OnComponentBeginOverlap.AddDynamic(
		this, &ATrigger_Base::BeginTriggerComponentOverlap);
	CollisionTriggerComponent->OnComponentEndOverlap.AddDynamic(
		this, &ATrigger_Base::EndTriggerComponentOverlap);


	//等一帧防止开始时候Actor无法Overlap
	GetWorld()->GetTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateUObject(this, &ATrigger_Base::BeginComponentOverlap));
}

void ATrigger_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ATrigger_Base::Tick(float DeltaSeconds)
{
}

void ATrigger_Base::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (TriggerData == nullptr) return;
	if (TriggerData->TriggerShape == nullptr) return;
	if (TriggerData->bUseLoadShape)
	{
		if (TriggerData->LoadShape == nullptr) return;
		if (auto ActorComponent = AddComponentByClass(TriggerData->LoadShape->GetPrimitiveComponentClass(), false,
		                                              FTransform::Identity, true))
		{
			if (auto PrimitiveComponent = Cast<UPrimitiveComponent>(ActorComponent))
			{
				PrimitiveComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
				TriggerData->LoadShape->PostAddComponent(PrimitiveComponent);
				FinishAddComponent(PrimitiveComponent, false, FTransform::Identity);
				CollisionLoadComponent = PrimitiveComponent;
			}
		}
	}
	if (auto ActorComponent = AddComponentByClass(TriggerData->TriggerShape->GetPrimitiveComponentClass(), false,
	                                              FTransform::Identity, true))
	{
		if (auto PrimitiveComponent = Cast<UPrimitiveComponent>(ActorComponent))
		{
			PrimitiveComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			TriggerData->TriggerShape->PostAddComponent(PrimitiveComponent);
			FinishAddComponent(PrimitiveComponent, false, FTransform::Identity);
			CollisionTriggerComponent = PrimitiveComponent;
		}
	}
}

void ATrigger_Base::BeginComponentOverlap()
{
	if (CollisionLoadComponent)
	{
		TArray<UPrimitiveComponent*> OutOverlappingComponents;
		CollisionLoadComponent->GetOverlappingComponents(OutOverlappingComponents);
		for (UPrimitiveComponent* Component : OutOverlappingComponents)
		{
			BeginLoadComponentOverlap(CollisionLoadComponent, Component->GetOwner(), Component, 0, false,
			                          FHitResult());
		}
	}
	if (CollisionTriggerComponent)
	{
		TArray<UPrimitiveComponent*> OutOverlappingComponents;
		CollisionTriggerComponent->GetOverlappingComponents(OutOverlappingComponents);
		for (UPrimitiveComponent* Component : OutOverlappingComponents)
		{
			BeginTriggerComponentOverlap(CollisionTriggerComponent, Component->GetOwner(), Component, 0, false,
			                             FHitResult());
		}
	}
}

void ATrigger_Base::BeginTriggerComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                 const FHitResult& SweepResult)
{
	if (EnterTriggerActors.Contains(OtherActor)) return;
	EnterTriggerActors.Add(OtherActor);
	if (OnTriggerComponentEnter(OtherActor, OtherComp))
	{
		OnTriggerEnter(OtherActor, OtherComp);
	}
}

void ATrigger_Base::EndTriggerComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!EnterTriggerActors.Contains(OtherActor)) return;
	EnterTriggerActors.Remove(OtherActor);
	if (OnTriggerComponentExit(OtherActor, OtherComp))
	{
		OnTriggerExit(OtherActor, OtherComp);
	}
}

void ATrigger_Base::BeginLoadComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                              const FHitResult& SweepResult)
{
	if (EnterLoadActors.Contains(OtherActor)) return;
	EnterLoadActors.Add(OtherActor);

	OnShowTriggerEnter(OtherActor, OtherComp);
}

void ATrigger_Base::EndLoadComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!EnterLoadActors.Contains(OtherActor)) return;
	EnterLoadActors.Remove(OtherActor);
	OnShowTriggerExit(OtherActor, OtherComp);
}

void ATrigger_Base::OnShowTriggerExit_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
}

void ATrigger_Base::OnShowTriggerEnter_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
}

bool ATrigger_Base::OnTriggerComponentEnter_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	if (!IsValid(TriggerData)) return false;
	if (TriggerActorArray.Contains(OtherActor)) return false;
	LastTriggerActor = OtherActor;
	return true;
}

bool ATrigger_Base::OnTriggerComponentExit_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	if (!IsValid(TriggerData)) return false;
	if (!TriggerActorArray.Contains(OtherActor)) return false;
	LastTriggerActor = OtherActor;
	return true;
}

void ATrigger_Base::OnTriggerEnter_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	TriggerActorArray.Add(OtherActor);
}

void ATrigger_Base::OnTriggerExit_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	TriggerActorArray.Remove(OtherActor);
}
