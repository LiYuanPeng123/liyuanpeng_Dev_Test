

#include "PatrolPath.h"

#include "ParolPathManagerSubsystem.h"
#include "Components/SplineComponent.h"


APatrolPath::APatrolPath()
{

	PrimaryActorTick.bCanEverTick = true;
	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SplineComp->SetupAttachment(RootComponent);
	SplineComp->bEditableWhenInherited = true;
}


void APatrolPath::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld())
	{
		if (UParolPathManagerSubsystem* PathSub = GetWorld()->GetSubsystem<UParolPathManagerSubsystem>())
		{
			PathSub->AddPatrolPath(this);
		}
	}
	
}

void APatrolPath::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		if (UParolPathManagerSubsystem* PathSub = GetWorld()->GetSubsystem<UParolPathManagerSubsystem>())
		{
			PathSub->RemovePatrolPath(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void APatrolPath::AddSplinePoint(const FVector& worldPos)
{
	SplineComp->AddSplinePoint(worldPos, ESplineCoordinateSpace::World);
}

void APatrolPath::SetSplinePoints(const TArray<FVector>& worldPos)
{
	SplineComp->SetSplinePoints(worldPos, ESplineCoordinateSpace::World);
}

