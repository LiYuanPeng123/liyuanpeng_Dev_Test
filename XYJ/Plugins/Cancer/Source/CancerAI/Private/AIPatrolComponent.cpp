#include "AIPatrolComponent.h"
#include "NavigationSystem.h"
#include "PatrolPath.h"
#include "Components/SplineComponent.h"
#include "NavFilters/NavigationQueryFilter.h"


UAIPatrolComponent::UAIPatrolComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PathToFollow = nullptr;
}


void UAIPatrolComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UAIPatrolComponent::TryGetNextWaypoint(FVector& outLocation)
{
	// 跟随样条路径
	if (PathToFollow)
	{
		const USplineComponent* path = PathToFollow->GetSplineComponent();
		if (!path)
		{
			return false;
		}
		const int32 NumPoints = path->GetNumberOfSplinePoints();
		if (NumPoints <= 0)
		{
			return false;
		}
		// 先规范化索引，再读取样条点
		if (PatrolIndex >= NumPoints || PatrolIndex < 0)
		{
			PatrolIndex = 0;
		}
		const FTransform waypoint = path->GetTransformAtSplinePoint(PatrolIndex, ESplineCoordinateSpace::World);
		// 下一个索引（循环）
		PatrolIndex = (PatrolIndex + 1) % NumPoints;

		FVector outLoc;
		// 优先投射到可导航网格
		if (UNavigationSystemV1::K2_ProjectPointToNavigation(this, waypoint.GetLocation(), outLoc, nullptr,
		                                                     UNavigationQueryFilter::StaticClass()))
		{
			outLocation = outLoc;
		}
		else if (!waypoint.GetLocation().IsNearlyZero())
		{
			outLocation = waypoint.GetLocation();
		}
		return true;
	}
	return false;
}
