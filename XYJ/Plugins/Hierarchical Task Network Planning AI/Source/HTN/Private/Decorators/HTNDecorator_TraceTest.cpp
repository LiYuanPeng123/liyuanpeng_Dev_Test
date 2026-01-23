// Copyright 2020-2025 Maksym Maisak. All Rights Reserved.

#include "Decorators/HTNDecorator_TraceTest.h"

#include "HTNObjectVersion.h"
#include "WorldStateProxy.h"

#include "AIController.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "Misc/ScopeExit.h"
#include "VisualLogger/VisualLogger.h"

UHTNDecorator_TraceTest::UHTNDecorator_TraceTest(const FObjectInitializer& Initializer) : Super(Initializer),
	bUseComplexCollision(false),
	bIgnoreSelf(true),
	TraceShape(EEnvTraceShape::Line),
	TraceExtentX(0.0f),
	TraceExtentY(0.0f),
	TraceExtentZ(0.0f),
	DrawDebugType(EDrawDebugTrace::None),
	DebugColor(FLinearColor::Red),
	DebugHitColor(FLinearColor::Green),
	DebugDrawTime(5.0f)
{
	NodeName = TEXT("Trace Test");
	
	TraceFromSource.InitializeBlackboardKeySelector(this, GET_MEMBER_NAME_CHECKED(ThisClass, TraceFromSource));
	TraceToSource.InitializeBlackboardKeySelector(this, GET_MEMBER_NAME_CHECKED(ThisClass, TraceToSource));
}

void UHTNDecorator_TraceTest::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FHTNObjectVersion::GUID);
	const int32 HTNObjectVersion = Ar.CustomVer(FHTNObjectVersion::GUID);

	if (HTNObjectVersion < FHTNObjectVersion::MakeTraceTestUseHTNLocationSource)
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		TraceFromSource.BlackboardKey = TraceFrom_DEPRECATED;
		TraceFromSource.Offset.Z = TraceFromZOffset_DEPRECATED;
		TraceToSource.BlackboardKey = TraceTo_DEPRECATED;
		TraceToSource.Offset.Z = TraceToZOffset_DEPRECATED;
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}
}

void UHTNDecorator_TraceTest::InitializeFromAsset(UHTN& Asset)
{
	Super::InitializeFromAsset(Asset);

	TraceFromSource.InitializeFromAsset(Asset);
	TraceToSource.InitializeFromAsset(Asset);
}

FString UHTNDecorator_TraceTest::GetStaticDescription() const
{
	static const UEnum* const TraceShapeEnum = StaticEnum<EEnvTraceShape::Type>();
	static const UEnum* const CollisionChannelEnum = StaticEnum<ECollisionChannel>();
	
	return FString::Printf(TEXT("%s: %s trace\nfrom %s\nto %s\non %s\nMust %s"), 
		*Super::GetStaticDescription(),
		*TraceShapeEnum->GetDisplayNameTextByValue(TraceShape.GetValue()).ToString(),
		*TraceFromSource.ToString(), *TraceToSource.ToString(),
		*CollisionChannelEnum->GetDisplayNameTextByValue(CollisionChannel.GetValue()).ToString(),
		IsInversed() ? TEXT("not hit") : TEXT("hit")
	);
}

bool UHTNDecorator_TraceTest::CalculateRawConditionValue(UHTNComponent& OwnerComp, uint8* NodeMemory, EHTNDecoratorConditionCheckType CheckType) const
{
	UWorldStateProxy* const WorldStateProxy = GetWorldStateProxy(OwnerComp, CheckType);
	if (!ensure(WorldStateProxy))
	{
		return false;
	}

	const FVector StartLocation = TraceFromSource.GetLocation(OwnerComp.GetOwner(), *WorldStateProxy);
	if (!FAISystem::IsValidLocation(StartLocation))
	{
		return false;
	}

	const FVector EndLocation = TraceToSource.GetLocation(OwnerComp.GetOwner(), *WorldStateProxy);
	if (!FAISystem::IsValidLocation(EndLocation))
	{
		return false;
	}

	const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(CollisionChannel);
	const FVector Extent(TraceExtentX, TraceExtentY, TraceExtentZ);
	FillActorsToIgnoreBuffer(OwnerComp, 
		WorldStateProxy->GetValueAsActor(TraceFromSource.BlackboardKey.SelectedKeyName),
		WorldStateProxy->GetValueAsActor(TraceToSource.BlackboardKey.SelectedKeyName));
	ON_SCOPE_EXIT { ActorsToIgnoreBuffer.Reset(); };

	FHitResult Hit;
	bool bHit = false;
	switch (TraceShape)
	{
	case EEnvTraceShape::Line:
		bHit = UKismetSystemLibrary::LineTraceSingle(&OwnerComp, StartLocation, EndLocation,
			TraceTypeQuery, bUseComplexCollision,
			ActorsToIgnoreBuffer,
			DrawDebugType, Hit, /*bIgnoreSelf=*/false, DebugColor, DebugHitColor, DebugDrawTime
		);
		break;

	case EEnvTraceShape::Box:
		bHit = UKismetSystemLibrary::BoxTraceSingle(&OwnerComp, StartLocation, EndLocation,
			Extent, (EndLocation - StartLocation).Rotation(),
			TraceTypeQuery, bUseComplexCollision,
			ActorsToIgnoreBuffer,
			DrawDebugType, Hit, /*bIgnoreSelf=*/false, DebugColor, DebugHitColor, DebugDrawTime
		);
		break;

	case EEnvTraceShape::Sphere:
		bHit = UKismetSystemLibrary::SphereTraceSingle(&OwnerComp, StartLocation, EndLocation,
			Extent.X,
			TraceTypeQuery, bUseComplexCollision,
			ActorsToIgnoreBuffer,
			DrawDebugType, Hit, /*bIgnoreSelf=*/false, DebugColor, DebugHitColor, DebugDrawTime
		);
		break;

	case EEnvTraceShape::Capsule:
		bHit = UKismetSystemLibrary::CapsuleTraceSingle(&OwnerComp, StartLocation, EndLocation,
			Extent.X, Extent.Z,
			TraceTypeQuery, bUseComplexCollision,
			ActorsToIgnoreBuffer,
			DrawDebugType, Hit, /*bIgnoreSelf=*/false, DebugColor, DebugHitColor, DebugDrawTime
		);
		break;

	default:
		break;
	}

	UE_VLOG_ARROW(OwnerComp.GetOwner(), LogHTN, Verbose, 
		StartLocation, EndLocation, 
		bHit ? DebugHitColor.ToFColor(/*sRGB=*/true) : DebugColor.ToFColor(/*sRGB=*/true), 
		TEXT("%s"), 
		*GetShortDescription()
	);
	return bHit;
}

void UHTNDecorator_TraceTest::FillActorsToIgnoreBuffer(UHTNComponent& OwnerComp, AActor* TraceFromActor, AActor* TraceToActor) const
{
	ActorsToIgnoreBuffer.Reset();
	if (bIgnoreSelf)
	{
		ActorsToIgnoreBuffer.Add(OwnerComp.GetOwner());
		if (const AAIController* const AIController = OwnerComp.GetAIOwner())
		{
			if (APawn* const Pawn = AIController->GetPawn())
			{
				ActorsToIgnoreBuffer.AddUnique(Pawn);
			}
		}
	}

	if (TraceFromActor)
	{
		ActorsToIgnoreBuffer.AddUnique(TraceFromActor);
	}

	if (TraceToActor)
	{
		ActorsToIgnoreBuffer.AddUnique(TraceToActor);
	}
}
