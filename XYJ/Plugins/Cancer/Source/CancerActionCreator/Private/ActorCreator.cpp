#include "ActorCreator.h"
#include "Action_DataAssetTemplate.h"
#include "CancerActionLibrary.h"
#include "Components/CapsuleComponent.h"


AActorCreator::AActorCreator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	RootComponent = SceneRootComponent;
	PreviewComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("PreviewComponent"));
	PreviewComponent->SetupAttachment(RootComponent);
	PreviewComponent->SetHiddenInGame(true);
}

void AActorCreator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
#if WITH_EDITOR
	if (!ActorData.IsNull())
	{
		UAction_DataAsset* Data = ActorData.LoadSynchronous();
		if (Data && Data->ActorClass)
		{
			UClass* PClass = Data->ActorClass;
			if (PreviewComponent)
			{
				if (PreviewComponent->GetChildActorClass() != PClass)
				{
					PreviewComponent->SetChildActorClass(PClass);
				}

				if (AActor* ChildActor = PreviewComponent->GetChildActor())
				{
					if (ACharacter* Character = Cast<ACharacter>(ChildActor))
					{
						float HalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
						PreviewComponent->SetRelativeLocation(FVector(0, 0, HalfHeight));
					}
				}
			}
		}
	}
	else
	{
		if (PreviewComponent)
		{
			PreviewComponent->SetChildActorClass(nullptr);
		}
	}
#endif
}

void AActorCreator::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(PreviewComponent))
	{
		PreviewComponent->SetHiddenInGame(true);
		PreviewComponent->DestroyComponent();
	}
	if (!ActorData.IsNull())
	{
		UAction_DataAsset* Data = ActorData.LoadSynchronous();
		UCancerActionLibrary::K2_CreatePawnFromActionData(this, Data, GetActorTransform(), {});
	}
}

#if WITH_EDITOR
void AActorCreator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property)
	{
		return;
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(AActorCreator, ActorData))
	{
		if (!IsTemplate() && !ActorData.IsNull() && PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			RerunConstructionScripts();
		}
	}
}
#endif
