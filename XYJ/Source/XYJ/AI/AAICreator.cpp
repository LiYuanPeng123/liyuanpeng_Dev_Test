#include "AAICreator.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "CancerActionLibrary.h"
#include "XYJ/Character/PawnData.h"
#include "XYJ/Character/XYJ_Character.h"
#include "Components/ChildActorComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogAicreator, Log, All);

UE_DISABLE_OPTIMIZATION

AAICreator::AAICreator(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	RootComponent = SceneRootComponent;
	PreviewComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("PreviewComponent"));
	PreviewComponent->SetupAttachment(RootComponent);
	PreviewComponent->SetHiddenInGame(true);
}

void AAICreator::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(PreviewComponent))
	{
		PreviewComponent->SetHiddenInGame(true);
		PreviewComponent->DestroyComponent();
	}
	StartLocation = GetActorLocation();
}

void AAICreator::OnShowTriggerEnter_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	Super::OnShowTriggerEnter_Implementation(OtherActor, OtherComp);

	if (!IsPlayerControlled(OtherActor))
		return;
	if (CachePawn)
		CachePawn->Destroy();

	// 记录控制器 并立即启动距离检测
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		CachedController = Cast<APlayerController>(Pawn->GetController());
	}
	StartUnloadCheck();

	UAction_DataAsset* ActionData = ActorData.LoadSynchronous();

	FRotator SpawnRotation = FRotator(0, GetActorRotation().Yaw, 0);
	FTransform SpawnTransform(SpawnRotation, StartLocation);
	CachePawn = UCancerActionLibrary::K2_CreatePawnFromActionData(this, ActionData, SpawnTransform, {});
}

void AAICreator::OnShowTriggerExit_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	Super::OnShowTriggerExit_Implementation(OtherActor, OtherComp);
}


void AAICreator::CheckUnloadCondition()
{
	// 如果控制器已经无效 直接卸载
	if (!CachedController.IsValid())
	{
		if (CachePawn)
		{
			CachePawn->Destroy();
			CachePawn = nullptr;
		}
		StopUnloadCheck();
		return;
	}

	FVector Loc = FVector::ZeroVector;
	if (APawn* PlayerPawn = CachedController->GetPawn())
	{
		Loc = PlayerPawn->GetActorLocation();
	}
	else
	{
		return;
	}

	bool bIsInside = false;

	if (CollisionLoadComponent)
	{
		if (UBoxComponent* BoxComp = Cast<UBoxComponent>(CollisionLoadComponent))
		{
			// Box检测需要转换到局部坐标系判断XYZ
			FVector LocalLoc = BoxComp->GetComponentTransform().InverseTransformPosition(Loc);
			FVector BoxExtent = BoxComp->GetScaledBoxExtent();

			// 增加一点容差（Buffer），避免在边界反复横跳
			float Tolerance = 100.0f;
			bIsInside = (FMath::Abs(LocalLoc.X) <= (BoxExtent.X + Tolerance)) &&
				(FMath::Abs(LocalLoc.Y) <= (BoxExtent.Y + Tolerance)) &&
				(FMath::Abs(LocalLoc.Z) <= (BoxExtent.Z + Tolerance));
		}
		else if (USphereComponent* SphereComp = Cast<USphereComponent>(CollisionLoadComponent))
		{
			// 球体检测
			float DistSquared = FVector::DistSquared(Loc, SphereComp->GetComponentLocation());
			float Radius = SphereComp->GetScaledSphereRadius();
			float Tolerance = 100.0f;
			bIsInside = DistSquared <= FMath::Square(Radius + Tolerance);
		}
		else
		{
			// 默认回退到Bounds检测
			float DistSquared = FVector::DistSquared(Loc, StartLocation);
			float UnloadDist = CollisionLoadComponent->Bounds.SphereRadius;
			bIsInside = DistSquared <= FMath::Square(UnloadDist * 1.5f); // 默认给大一点
		}
	}
	else
	{
		// 如果没有组件，使用默认大距离
		float DistSquared = FVector::DistSquared(Loc, StartLocation);
		bIsInside = DistSquared <= FMath::Square(5000.0f);
	}

	// 如果不在范围内，执行卸载
	if (!bIsInside)
	{
		if (CachePawn)
		{
			CachePawn->Destroy();
			CachePawn = nullptr;
		}
		StopUnloadCheck();
	}
}

void AAICreator::StartUnloadCheck()
{
	if (GetWorld())
	{
		//每秒检测一次
		GetWorld()->GetTimerManager().SetTimer(UnloadCheckTimer, this, &AAICreator::CheckUnloadCondition, 1.0f, true);
	}
}

void AAICreator::StopUnloadCheck()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(UnloadCheckTimer);
	}
}

void AAICreator::OnConstruction(const FTransform& Transform)
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

bool AAICreator::IsPlayerControlled(AActor* InActor) const
{
	if (APawn* Pawn = Cast<APawn>(InActor))
	{
		if (APlayerController* Controller = Cast<APlayerController>(Pawn->GetController()))
		{
			return true;
		}
	}
	return false;
}

#if WITH_EDITOR
void AAICreator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property)
	{
		return;
	}

	
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(AAICreator, ActorData))
	{
		if (!IsTemplate() && !ActorData.IsNull() && PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			RerunConstructionScripts();
		}
	}
}
#endif
UE_ENABLE_OPTIMIZATION
