#include "GameFramework/CancerMeleeScan.h"
#include "CancerAbilitySetting.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/CancerDamageType.h"


UE_DISABLE_OPTIMIZATION

UCancerMeleeScan* UCancerMeleeScan::NewInstance(TSubclassOf<UCancerMeleeScan> MeleeScanClass, AActor* Owner,
                                                AActor* Instigator, UMeshComponent* ScanMesh,
                                                const TArray<FName>& SocketNames,
                                                const TMap<FName, FVector>& TraceSocketOffset,
                                                TArray<TEnumAsByte<ECollisionChannel>> ScanChannels,
                                                EMeleeScanMode ScanMode,
                                                FVector ShapeDimensions,
                                                float TraceRadius,
                                                float Duration)
{
	const UClass* SafeMeleeScanClass = IsValid(MeleeScanClass) ? MeleeScanClass.Get() : StaticClass();
	UCancerMeleeScan* MeleeScan = NewObject<UCancerMeleeScan>(Owner, SafeMeleeScanClass);

	MeleeScan->ScanOwner = Owner;
	MeleeScan->Instigator = Cast<APawn>(Instigator);
	MeleeScan->ScanMesh = ScanMesh;
	MeleeScan->SocketNames = SocketNames;
	MeleeScan->TraceSocketOffset = TraceSocketOffset;
	MeleeScan->ScanChannels = ScanChannels;
	MeleeScan->ScanMode = ScanMode;
	MeleeScan->Duration = Duration;
	MeleeScan->ShapeDimensions = ShapeDimensions;
	MeleeScan->TraceRadius = TraceRadius;
	switch (ScanMode)
	{
	case EMeleeScanMode::BoxSweep:
		{
			MeleeScan->ScanShape = FCollisionShape::MakeBox(ShapeDimensions);
			break;
		}
	case EMeleeScanMode::CapsuleSweep:
		{
			MeleeScan->ScanShape = FCollisionShape::MakeCapsule(ShapeDimensions);
			break;
		}
	case EMeleeScanMode::SphereSweep:
		{
			const float SphereRadius = ShapeDimensions.X;
			MeleeScan->ScanShape = FCollisionShape::MakeSphere(SphereRadius);
			break;
		}
	case EMeleeScanMode::BoxWithSelf:
		{
			// 原地盒体重叠/厚度检测使用盒体
			MeleeScan->ScanShape = FCollisionShape::MakeBox(ShapeDimensions);
			break;
		}
	case EMeleeScanMode::SphereWithSelf:
		{
			// 原地球体重叠/厚度检测使用球体（X 为半径）
			const float SphereRadius = ShapeDimensions.X;
			MeleeScan->ScanShape = FCollisionShape::MakeSphere(SphereRadius);
			break;
		}
	default: ;
	}

	MeleeScan->SnapshotSocketPositions();
	return MeleeScan;
}

bool UCancerMeleeScan::HasValidScanData() const
{
	if (!IsValid(ScanOwner))
	{
		return false;
	}
	if (ScanMesh == nullptr)
	{
		return false;
	}
	if (SocketNames.IsEmpty())
	{
		return false;
	}
	for (const FName& SocketName : SocketNames)
	{
		if (!ScanMesh->DoesSocketExist(SocketName))
		{
			return false;
		}
	}

	return true;
}


TArray<AActor*> UCancerMeleeScan::GetIgnoredActors_Implementation() const
{
	//忽略自己
	TArray<AActor*> IgnoredActors;
	if (IsValid(ScanOwner))
	{
		//ScanOwner->GetAttachedActors(IgnoredActors);

		IgnoredActors.Add(ScanOwner);
	}
	return IgnoredActors;
}

void UCancerMeleeScan::ScanForTargets_Implementation(TArray<FHitResult>& OutHits)
{
	if (!IsValid(ScanOwner)) return;

	const UWorld* World = ScanOwner->GetWorld();
	if (!ScanMesh)return;
	if (!IsValid(World) || LastSocketPositions.IsEmpty()) return;

	const FCollisionObjectQueryParams ObjectParams = CreateObjectParams();
	const FCollisionQueryParams TraceParams = CreateTraceParams();
	const EDrawDebugTrace::Type DrawDebugType = bEnableDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	const float DebugDrawTime = UCancerAbilitySetting::Get()->MeleeScanDebugDuration;
	// 同帧基线：仅对本次调用期间新增的命中去重
	const int32 OutHitsBaseline = OutHits.Num();


	auto PerformSphereSweep = [&](const FVector& InLastA, const FVector& InLastB, const FVector& InCurA,
	                              const FVector& InCurB, TArray<FHitResult> OutHitResults)
	{
		const float Radius = FMath::Max(TraceRadius, 1.0f);
		const float SegmentLen = FVector::Dist(InCurA, InCurB);

		// 根据长度和半径计算分段数，确保覆盖
		const float StepSize = FMath::Max(Radius, 1.0f);
		const int32 Steps = FMath::Max(1, FMath::CeilToInt(SegmentLen / StepSize));

		for (int32 i = 0; i <= Steps; ++i)
		{
			const float Alpha = (float)i / (float)Steps;
			const FVector Start = FMath::Lerp(InLastA, InLastB, Alpha);
			const FVector End = FMath::Lerp(InCurA, InCurB, Alpha);

			TArray<FHitResult> StepHits;
			// 从上一帧位置扫掠到当前帧位置
			World->SweepMultiByObjectType(
				StepHits,
				Start,
				End,
				FQuat::Identity,
				ObjectParams,
				FCollisionShape::MakeSphere(Radius),
				TraceParams
			);

			OutHitResults.Append(StepHits);

#if WITH_EDITOR
			if (bEnableDebug)
			{
				const float LifeTime = UCancerAbilitySetting::Get()->MeleeScanDebugDuration;
				// 绘制每一段的扫掠胶囊体（或线段）
				const FVector SegDir = End - Start;
				const float SegLen = SegDir.Size();
				if (SegLen > KINDA_SMALL_NUMBER)
				{
					const FVector Mid = (Start + End) * 0.5f;
					const float HalfHeight = SegLen * 0.5f + Radius;
					const FQuat Rot = FRotationMatrix::MakeFromZ(SegDir).ToQuat();
					::DrawDebugCapsule(World, Mid, HalfHeight, Radius, Rot, FColor::Red, false, LifeTime);
				}
				else
				{
					::DrawDebugSphere(World, End, Radius, 8, FColor::Red, false, LifeTime);
				}
			}
#endif
		}
	};
	

	for (int32 SocketIdx = 0; SocketIdx < SocketNames.Num(); ++SocketIdx)
	{
		const FName SocketName = SocketNames[SocketIdx];
		if (SocketName.IsNone())continue;
		if (!LastSocketPositions.Contains(SocketName))
			continue;
		const FVector LastSocketPosition = *LastSocketPositions.Find(SocketName);
		const FVector SocketPosition = GetAdjustedSocketPosition(SocketName);
		const FQuat SocketRotation = ScanMesh->GetSocketQuaternion(SocketName);
		FQuat LastSocketRotation = LastSocketRotations.FindRef(SocketName);
		if (!LastSocketRotations.Contains(SocketName))
		{
			// 首次没有缓存时，使用当前旋转作为上一帧旋转的回退，避免异常角度计算
			LastSocketRotation = SocketRotation;
		}
		LastSocketPositions[SocketName] = SocketPosition;
		LastSocketRotations.Add(SocketName, SocketRotation);

		TArray<FHitResult> HitResults;

		if (ScanMode == EMeleeScanMode::LineTrace)
		{
			const auto PerformLineTrace = [&](const int32 OtherIdx)
			{
				const FVector OtherSocketPosition = GetAdjustedSocketPosition(SocketNames[OtherIdx]);
				World->LineTraceMultiByObjectType(HitResults, SocketPosition, OtherSocketPosition, ObjectParams,
				                                  TraceParams);
				DrawScanLine(LastSocketPosition, OtherSocketPosition);
			};

			const int32 LastIdx = SocketIdx - 1;
			const int32 NextIdx = SocketIdx + 1;

			if (SocketNames.IsValidIndex(LastIdx))
			{
				PerformLineTrace(LastIdx);
			}
			else if (SocketNames.IsValidIndex(NextIdx))
			{
				PerformLineTrace(NextIdx);
			}

			World->LineTraceMultiByObjectType(HitResults, LastSocketPosition, SocketPosition, ObjectParams,
			                                  TraceParams);
			DrawScanLine(LastSocketPosition, SocketPosition);
		}
		//插槽之间的碰撞盒检测
		else if (ScanMode == EMeleeScanMode::BoxWithSelf)
		{
			// 成对处理：(0,1), (2,3), ...，仅在对内第一个索引执行
			const int32 PairIndexA = (SocketIdx % 2 == 0) ? SocketIdx : SocketIdx - 1;
			if (SocketIdx != PairIndexA)
			{
				continue;
			}
			const int32 PairIndexB = PairIndexA + 1;
			if (!SocketNames.IsValidIndex(PairIndexB))
			{
				continue;
			}

			const FName SocketNameA = SocketNames[PairIndexA];
			const FName SocketNameB = SocketNames[PairIndexB];

			const FVector CurA = GetAdjustedSocketPosition(SocketNameA);
			const FVector CurB = GetAdjustedSocketPosition(SocketNameB);

			const FVector* LastAPtr = LastSocketPositions.Find(SocketNameA);
			const FVector* LastBPtr = LastSocketPositions.Find(SocketNameB);

			const FVector LastA = LastAPtr ? *LastAPtr : CurA;
			const FVector LastB = LastBPtr ? *LastBPtr : CurB;

			const float Radius = FMath::Max(TraceRadius, 1.0f);
			const float SegmentLen = FVector::Dist(CurA, CurB);

			// 根据长度和半径计算分段数，确保覆盖
			const float StepSize = FMath::Max(Radius, 1.0f);
			const int32 Steps = FMath::Max(1, FMath::CeilToInt(SegmentLen / StepSize));

			const FVector SegmentDir = (CurB - CurA).GetSafeNormal();
			const FRotator BoxRot = SegmentDir.IsNearlyZero()
				                        ? FRotator::ZeroRotator
				                        : FRotationMatrix::MakeFromX(SegmentDir).Rotator();
			const FVector BoxHalfSize(Radius, Radius, Radius);

			for (int32 i = 0; i <= Steps; ++i)
			{
				const float Alpha = (float)i / (float)Steps;
				const FVector Start = FMath::Lerp(LastA, LastB, Alpha);
				const FVector End = FMath::Lerp(CurA, CurB, Alpha);

				TArray<FHitResult> StepHits;
				// 从上一帧位置扫掠到当前帧位置
				World->SweepMultiByObjectType(
					StepHits,
					Start,
					End,
					BoxRot.Quaternion(),
					ObjectParams,
					FCollisionShape::MakeBox(BoxHalfSize),
					TraceParams
				);

				HitResults.Append(StepHits);

#if WITH_EDITOR
				if (bEnableDebug)
				{
					const float LifeTime = UCancerAbilitySetting::Get()->MeleeScanDebugDuration;
					const FVector SegDir = End - Start;
					const float SegLen = SegDir.Size();
					if (SegLen > KINDA_SMALL_NUMBER)
					{
						const FVector Mid = (Start + End) * 0.5f;
						const float HalfLength = SegLen * 0.5f + Radius;
						const FVector SweptBoxExtent(Radius, Radius, HalfLength);
						const FQuat SweptBoxRot = FRotationMatrix::MakeFromZ(SegDir).ToQuat();
						::DrawDebugBox(World, Mid, SweptBoxExtent, SweptBoxRot, FColor::Red, false, LifeTime);
					}
					else
					{
						::DrawDebugBox(World, End, BoxHalfSize, BoxRot.Quaternion(), FColor::Red, false, LifeTime);
					}
				}
#endif
			}
		}
		//插槽之间的碰撞球体检测
		else if (ScanMode == EMeleeScanMode::SphereWithSelf)
		{
			// 成对处理：(0,1), (2,3), ...，仅在对内第一个索引执行
			const int32 PairIndexA = (SocketIdx % 2 == 0) ? SocketIdx : SocketIdx - 1;
			if (SocketIdx != PairIndexA)
			{
				continue;
			}
			const int32 PairIndexB = PairIndexA + 1;
			if (!SocketNames.IsValidIndex(PairIndexB))
			{
				continue;
			}

			const FName SocketNameA = SocketNames[PairIndexA];
			const FName SocketNameB = SocketNames[PairIndexB];

			const FVector CurA = GetAdjustedSocketPosition(SocketNameA);
			const FVector CurB = GetAdjustedSocketPosition(SocketNameB);

			const FVector* LastAPtr = LastSocketPositions.Find(SocketNameA);
			const FVector* LastBPtr = LastSocketPositions.Find(SocketNameB);

			const FVector LastA = LastAPtr ? *LastAPtr : CurA;
			const FVector LastB = LastBPtr ? *LastBPtr : CurB;

			const float Radius = FMath::Max(TraceRadius, 1.0f);
			const float SegmentLen = FVector::Dist(CurA, CurB);

			// 根据长度和半径计算分段数，确保覆盖
			const float StepSize = FMath::Max(Radius, 1.0f);
			const int32 Steps = FMath::Max(1, FMath::CeilToInt(SegmentLen / StepSize));

			for (int32 i = 0; i <= Steps; ++i)
			{
				const float Alpha = (float)i / (float)Steps;
				const FVector Start = FMath::Lerp(LastA, LastB, Alpha);
				const FVector End = FMath::Lerp(CurA, CurB, Alpha);

				TArray<FHitResult> StepHits;
				// 从上一帧位置扫掠到当前帧位置
				World->SweepMultiByObjectType(
					StepHits,
					Start,
					End,
					FQuat::Identity,
					ObjectParams,
					FCollisionShape::MakeSphere(Radius),
					TraceParams
				);

				HitResults.Append(StepHits);

#if WITH_EDITOR
				if (bEnableDebug)
				{
					const float LifeTime = UCancerAbilitySetting::Get()->MeleeScanDebugDuration;
					// 绘制每一段的扫掠胶囊体
					const FVector SegDir = End - Start;
					const float SegLen = SegDir.Size();
					if (SegLen > KINDA_SMALL_NUMBER)
					{
						const FVector Mid = (Start + End) * 0.5f;
						const float HalfHeight = SegLen * 0.5f + Radius;
						const FQuat Rot = FRotationMatrix::MakeFromZ(SegDir).ToQuat();
						::DrawDebugCapsule(World, Mid, HalfHeight, Radius, Rot, FColor::Red, false, LifeTime);
					}
					else
					{
						::DrawDebugSphere(World, End, Radius, 8, FColor::Red, false, LifeTime);
					}
				}
#endif
			}
		}
		else
		{
			if (bAdaptiveSubstepping)
			{
				PerformSweepWithSubsteps(World, LastSocketPosition, SocketPosition, LastSocketRotation, SocketRotation,
				                         ObjectParams, TraceParams, HitResults);
			}
			else
			{
				World->SweepMultiByObjectType(HitResults, LastSocketPosition, SocketPosition, SocketRotation,
				                              ObjectParams,
				                              ScanShape, TraceParams);
				DrawScanSweep(SocketPosition, SocketRotation);
			}
		}

		for (const FHitResult& NewHit : HitResults)
		{
			const AActor* NewActor = NewHit.GetActor();
			// 同帧内始终去重：仅比较本次调用期间新增的 OutHits
			bool bDuplicateInFrame = false;
			if (NewActor)
			{
				for (int32 Idx = OutHitsBaseline; Idx < OutHits.Num(); ++Idx)
				{
					if (OutHits[Idx].GetActor() == NewActor)
					{
						bDuplicateInFrame = true;

						// 优化：选择最佳命中点
						// 1. 优先选择非穿透起始点（bStartPenetrating=false）
						// 2. 选择距离攻击者更近的点（解决背面命中问题）
						if (IsValid(ScanOwner))
						{
							FHitResult& OldHit = OutHits[Idx];

							const bool bOldIsPenetrating = OldHit.bStartPenetrating;
							const bool bNewIsPenetrating = NewHit.bStartPenetrating;

							bool bShouldReplace = false;

							// 优先保留正常的碰撞检测结果，而不是初始重叠
							if (bOldIsPenetrating && !bNewIsPenetrating)
							{
								bShouldReplace = true;
							}
							// 如果性质相同，则比较距离
							else if (bOldIsPenetrating == bNewIsPenetrating)
							{
								const FVector OwnerLoc = ScanOwner->GetActorLocation();
								const float OldDistSq = FVector::DistSquared(OldHit.ImpactPoint, OwnerLoc);
								const float NewDistSq = FVector::DistSquared(NewHit.ImpactPoint, OwnerLoc);

								// 如果新命中点更近，则替换（通常意味着击中了正面而不是背面）
								if (NewDistSq < OldDistSq)
								{
									bShouldReplace = true;
								}
							}

							if (bShouldReplace)
							{
								OldHit = NewHit;
								if (bEnableDebug)
								{
									UE_LOG(LogTemp, Verbose, TEXT("[MeleeScan] 优化命中点: %s (Dist: %.1f -> %.1f)"),
									       *GetNameSafe(NewActor),
									       FVector::Dist(OutHits[Idx].ImpactPoint, ScanOwner->GetActorLocation()),
									       FVector::Dist(NewHit.ImpactPoint, ScanOwner->GetActorLocation()));
								}
							}
						}
						break;
					}
				}
			}

			bool bDuplicateAcrossFrames = false;
			if (!bDuplicateInFrame && !bAllowMultipleHitsOnSameTarget && NewActor)
			{
				bDuplicateAcrossFrames = ConsolidatedHits.ContainsByPredicate([&](const FHitResult& Hit)
				{
					return Hit.GetActor() == NewActor;
				});
			}

			const bool bShouldAddHit = !(bDuplicateInFrame || bDuplicateAcrossFrames);

			if (!bShouldAddHit && bEnableDebug)
			{
				UE_LOG(LogTemp, Warning, TEXT("[MeleeScan] 跳过重复目标: %s（原因=%s）"),
				       *GetNameSafe(NewActor),
				       bDuplicateInFrame ? TEXT("当前帧重复") : TEXT("历史重复"));
			}

			if (bShouldAddHit)
			{
				OutHits.Add(NewHit);
				ConsolidatedHits.Add(NewHit);
				if (bEnableDebug)
				{
					UE_LOG(LogTemp, Warning, TEXT("[MeleeScan] 新增目标: %s"), *GetNameSafe(NewActor));
				}
				DrawNewTarget(NewHit);
			}
		}
	}
}

void UCancerMeleeScan::SnapshotSocketPositions()
{
	for (const FName& SocketName : SocketNames)
	{
		if (!SocketName.IsNone() && ScanMesh->DoesSocketExist(SocketName)
		)
		{
			LastSocketPositions.Add(SocketName, GetAdjustedSocketPosition(SocketName));
			LastSocketRotations.Add(SocketName, ScanMesh->GetSocketQuaternion(SocketName));
		}
	}
}

FCollisionObjectQueryParams UCancerMeleeScan::CreateObjectParams() const
{
	FCollisionObjectQueryParams QueryParams;
	for (const TEnumAsByte<ECollisionChannel>& Channel : ScanChannels)
	{
		if (QueryParams.IsValidObjectQuery(Channel))
		{
			QueryParams.AddObjectTypesToQuery(Channel);
		}
	}

	return QueryParams;
}

FCollisionQueryParams UCancerMeleeScan::CreateTraceParams() const
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(GetCombatTargets));
	TraceParams.TraceTag = TEXT("MeleeScanTrace");
	TraceParams.bReturnPhysicalMaterial = false; // Disable unless specifically needed
	TraceParams.bTraceComplex = bTraceComplex;
	// 对 Sweep 查询启用初始重叠检测，避免起始位置已重叠时漏检
	TraceParams.bFindInitialOverlaps = true;

	TArray<AActor*> IgnoredActors = GetIgnoredActors();
	for (const AActor* AttachedActor : IgnoredActors)
	{
		TraceParams.AddIgnoredActor(AttachedActor);
	}

	return TraceParams;
}

void UCancerMeleeScan::DrawScanLine(const FVector& StartPosition, const FVector& EndPosition) const
{
#if WITH_EDITOR
	if (bEnableDebug)
	{
		const UWorld* World = ScanOwner->GetWorld();
		const float LifeTime = UCancerAbilitySetting::Get()->MeleeScanDebugDuration;
		::DrawDebugLine(World, StartPosition, EndPosition, FColor::Red, false, LifeTime);
	}
#endif
}

void UCancerMeleeScan::DrawScanSweep(const FVector& StartPosition, const FQuat& Rotation) const
{
#if WITH_EDITOR
	if (bEnableDebug)
	{
		const UWorld* World = ScanOwner->GetWorld();

		constexpr bool bPersistentLines = false;
		const float LifeTime = UCancerAbilitySetting::Get()->MeleeScanDebugDuration;

		if (ScanShape.IsBox())
		{
			const FVector Extent = ScanShape.GetExtent();
			::DrawDebugBox(World, StartPosition, Extent, Rotation, FColor::Red, bPersistentLines, LifeTime);
		}
		else if (ScanShape.IsCapsule())
		{
			const float HalfHeight = ScanShape.GetCapsuleHalfHeight();
			const float Radius = ScanShape.GetCapsuleRadius();
			::DrawDebugCapsule(World, StartPosition, HalfHeight, Radius, Rotation, FColor::Red, bPersistentLines,
			                   LifeTime);
		}
		else if (ScanShape.IsSphere())
		{
			const float Radius = ScanShape.GetSphereRadius();
			::DrawDebugSphere(World, StartPosition, Radius, 8.f, FColor::Red, bPersistentLines, LifeTime);
		}
	}
#endif
}

void UCancerMeleeScan::DrawNewTarget(const FHitResult& NewHit) const
{
#if WITH_EDITOR
	if (bEnableDebug)
	{
		const float LifeTime = UCancerAbilitySetting::Get()->MeleeScanDebugDuration;
		const UWorld* World = ScanOwner->GetWorld();
		const FVector StartPosition = NewHit.Location;
		DrawDebugSphere(World, StartPosition, 10.f, 12.f, FColor::Emerald, false, LifeTime);

		if (IsValid(GEngine))
		{
			const FString Message = FString::Printf(TEXT("Hit: %s (%s)"), *GetNameSafe(NewHit.GetActor()),
			                                        *GetNameSafe(NewHit.GetComponent()));
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, LifeTime, FColor::Green, Message);
		}
	}
#endif
}

FVector UCancerMeleeScan::GetAdjustedSocketPosition(const FName& SocketName) const
{
	if (!ScanMesh || SocketName.IsNone())
	{
		return FVector::ZeroVector;
	}
	const FVector Base = ScanMesh->GetSocketLocation(SocketName);
	const FVector* Offset = TraceSocketOffset.Find(SocketName);
	if (!Offset)
	{
		return Base;
	}
	const FTransform SocketTM = ScanMesh->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_World);
	const FVector Adjusted = Base + SocketTM.TransformVector(*Offset);
	return Adjusted;
}

void UCancerMeleeScan::PerformSweepWithSubsteps(
	const UWorld* World,
	const FVector& Start,
	const FVector& End,
	const FQuat& StartRot,
	const FQuat& EndRot,
	const FCollisionObjectQueryParams& ObjectParams,
	const FCollisionQueryParams& TraceParams,
	TArray<FHitResult>& OutHits
) const
{
	const float Distance = FVector::Dist(Start, End);
	const float AngleRad = StartRot.AngularDistance(EndRot);
	const float AngleDeg = FMath::RadiansToDegrees(AngleRad);

	int32 StepsByDist = FMath::CeilToInt(Distance / FMath::Max(1.0f, MaxSweepStepDistance));
	int32 StepsByAngle = FMath::CeilToInt(AngleDeg / FMath::Max(1.0f, MaxSweepStepAngleDeg));
	int32 Steps = FMath::Clamp(FMath::Max(StepsByDist, StepsByAngle), 1, MaxSubsteps);

	FVector PrevPos = Start;
	FQuat PrevRot = StartRot;
	for (int32 i = 1; i <= Steps; ++i)
	{
		const float T = static_cast<float>(i) / static_cast<float>(Steps);
		const FVector CurrPos = FMath::Lerp(Start, End, T);
		const FQuat CurrRot = FQuat::Slerp(StartRot, EndRot, T);

		TArray<FHitResult> StepHits;
		const bool bHit = World->SweepMultiByObjectType(
			StepHits,
			PrevPos,
			CurrPos,
			CurrRot,
			ObjectParams,
			ScanShape,
			TraceParams
		);

		if (bHit)
		{
			OutHits.Append(StepHits);
		}

		DrawScanSweep(CurrPos, CurrRot);
		PrevPos = CurrPos;
		PrevRot = CurrRot;
	}
}

UE_ENABLE_OPTIMIZATION
