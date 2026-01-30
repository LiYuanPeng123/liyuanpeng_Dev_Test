// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CancerLockingTargetComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerLockingFunctionLibrary.h"
#include "CancerLockingSubsystem.h"
#include "CancerLockTargetSetting.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CancerLockingPointComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


UCancerLockingTargetComponent::UCancerLockingTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCancerLockingTargetComponent::BeginPlay()
{
	Super::BeginPlay();
	//初始化UI组件
	if (!LockingPointWidgetClass)
	{
		LockingPointWidgetClass = UCancerLockTargetSetting::Get()->DefaultLockingPointWidgetClass.LoadSynchronous();
	}
	//没有就从默认设置中加载
	if (!LockingWidgetComponentClass)
	{
		LockingWidgetComponentClass = UCancerLockTargetSetting::Get()->LockingWidgetComponentClass.LoadSynchronous();
	}
	if (LockingWidgetComponentClass)
	{
		LockingPointWidgetComponent = NewObject<UWidgetComponent>(GetOwner(),LockingWidgetComponentClass);
		LockingPointWidgetComponent->SetupAttachment(GetOwner()->GetRootComponent());
		LockingPointWidgetComponent->SetWidgetClass(LockingPointWidgetClass);
		LockingPointWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		LockingPointWidgetComponent->SetVisibility(false);
		LockingPointWidgetComponent->RegisterComponent();
		/*LockingPointWidgetComponent->SetDrawSize()*/
	}
}

void UCancerLockingTargetComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//超出最大距离取消锁定
	if (CurLockingPointComponent)
	{
		// 当目标主动告知无法锁定时 取消锁定状态
		if (CurLockingPointComponent->IsOpen == false)
		{
			SetLockingPointComponent(nullptr);
		}
		else
		{
			const float Distance = FVector::Distance(GetOwner()->GetActorLocation(),
													 CurLockingPointComponent->GetComponentLocation());
			if (Distance >= MaxLockingDistance)
			{
				SetLockingPointComponent(nullptr);
			}
		}
	}

	// 自动切换目标：如果当前目标死亡（拥有dead标签），尝试搜索附近目标
	if (CurLockingPointComponent)
	{
		bool bIsDead = false;
		if (const AActor* Owner = CurLockingPointComponent->GetOwner())
		{
			if (Owner->ActorHasTag(FName("dead")))
			{
				bIsDead = true;
			}
			else if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
			{
				if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("dead"), false)) ||
					ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dead"), false)))
				{
					bIsDead = true;
				}
			}
		}

		if (bIsDead)
		{
			TArray<FFindLockingPointInfo> Candidates = FindLockingPointArray();
			// 排除当前已死亡的目标
			for (int32 i = Candidates.Num() - 1; i >= 0; --i)
			{
				if (Candidates[i].LockingPointComponent == CurLockingPointComponent)
				{
					Candidates.RemoveAt(i);
				}
			}

			if (Candidates.Num() > 0)
			{
				if (UCancerLockingPointComponent* NewTarget = FindLockingPointByDistance(Candidates))
				{
					SetLockingPointComponent(NewTarget);
				}
				else
				{
					SetLockingPointComponent(nullptr);
				}
			}
			else
			{
				SetLockingPointComponent(nullptr);
			}
		}
	}

	if(APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (!Pawn->IsPlayerControlled())
		{
			if (LockingPointWidgetComponent)
			{
				LockingPointWidgetComponent->SetVisibility(false);
			}
			
		}
	}
}


AActor* UCancerLockingTargetComponent::GetLockTarget() const
{
	if (CurLockingPointComponent)
	{
		return CurLockingPointComponent->GetOwner();
	}
	return nullptr;
}

void UCancerLockingTargetComponent::PassiveCancellationLock()
{
 	for (auto It = LockingSelfArray.CreateIterator(); It; ++It)
	{
		if (UCancerLockingTargetComponent* LockingComponent = *It)
		{
			LockingComponent->SetLockingPointComponent(nullptr);
		}
	}
}

void UCancerLockingTargetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//当组件销毁 通知所有锁定者取消锁定
	PassiveCancellationLock();
}

void UCancerLockingTargetComponent::AddLockingSelfComponent(UCancerLockingTargetComponent* LockingComponent,
	UCancerLockingPointComponent* LockingPointComponent)
{
	LockingSelfArray.Add(LockingComponent);
}

void UCancerLockingTargetComponent::RemoveLockingSelfComponent(UCancerLockingTargetComponent* LockingComponent,
	UCancerLockingPointComponent* LockingPointComponent)
{
	LockingSelfArray.Remove(LockingComponent);
}

void UCancerLockingTargetComponent::SetLockingPointComponent_Implementation(
	UCancerLockingPointComponent* LockingPointComponent)
{
	if (LockingPointComponent)
	{
		if (!IsCanLock(LockingPointComponent))return;
	}
	//如果锁定目标是同一个那就直接返回
	if (CurLockingPointComponent == LockingPointComponent) return;
	//通知上一个锁定者取消锁定
	if (CurLockingPointComponent)
	{
		if (UCancerLockingTargetComponent* LockingComponent = UCancerLockingFunctionLibrary::GetLockingTargetComponent(
			CurLockingPointComponent->GetOwner()))
		{
			LockingComponent->RemoveLockingSelfComponent(this, LockingPointComponent);
		}
	}
	//新的锁定
	CurLockingPointComponent = LockingPointComponent;
	//通知当前锁定者被锁定
	if (CurLockingPointComponent)
	{
		if (UCancerLockingTargetComponent* LockingComponent = UCancerLockingFunctionLibrary::GetLockingTargetComponent(
			CurLockingPointComponent->GetOwner()))
		{
			LockingComponent->AddLockingSelfComponent(this, LockingPointComponent);
		}
	}
	//锁定点通知
	OnLockingPointChangeDelegate.Broadcast(LockingPointComponent);
	//设置锁定UI
	if (LockingPointWidgetComponent)
	{
		if (CurLockingPointComponent)
		{
			LockingPointWidgetComponent->SetRelativeLocation(FVector::ZeroVector);
			LockingPointWidgetComponent->AttachToComponent(CurLockingPointComponent,
														   FAttachmentTransformRules(
															   EAttachmentRule::KeepRelative, false));
			LockingPointWidgetComponent->SetVisibility(true);
		}
		else
		{
			LockingPointWidgetComponent->SetVisibility(false);
		}
	}
}

void UCancerLockingTargetComponent::FinishLockingPointComponent_Implementation(
	)
{
	SetLockingPointComponent(nullptr);
}

bool UCancerLockingTargetComponent::IsLockTarget_Implementation()
{
	return CurLockingPointComponent != nullptr; 
}

TArray<FFindLockingPointInfo> UCancerLockingTargetComponent::FindLockingPointArray()
{
	TArray<FFindLockingPointInfo> LockingPointDataArray;
	//获取有效锁定点
	if (UCancerLockingSubsystem* LockingSubsystem = GetWorld()->GetSubsystem<UCancerLockingSubsystem>())
	{
		for (UCancerLockingPointComponent* LockingPointComponent : LockingSubsystem->GetAllLockingPointArray())
		{
			if (IsCanLock(LockingPointComponent))
			{
				FFindLockingPointInfo FindLockingPointData = FFindLockingPointInfo();
				FindLockingPointData.LockingPointComponent = LockingPointComponent;
				FindLockingPointData.Distance = FVector::Distance(GetOwner()->GetActorLocation(),
																  LockingPointComponent->GetComponentLocation());

				APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
				const FVector2D ScreenSize = UWidgetLayoutLibrary::GetViewportSize(PlayerController);
				const FVector2D ScreenCenterPoint = FVector2D(ScreenSize.X * 0.5f, ScreenSize.Y * 0.5f);
				UGameplayStatics::ProjectWorldToScreen(PlayerController,
													   FindLockingPointData.LockingPointComponent->
																			GetComponentLocation(),
													   FindLockingPointData.Screen2DPoint);
				FindLockingPointData.ScreenPointDistance = FVector2D::Distance(
					ScreenCenterPoint, FindLockingPointData.Screen2DPoint);
				//在锁定范围内，并且在屏幕范围内
				float LockDistance = FMath::Min(/*UCancerLockTargetSetting::Get()->LockingConfig.MaxLockingDistance*/
					MaxLockingDistance, LockingPointComponent->MaxUnLockingDistance);
				if (FindLockingPointData.Distance < LockDistance &&
					IsInScreenViewport(LockingPointComponent->GetComponentLocation()) &&
					IsNoObstruction(LockingPointComponent))
				{
					LockingPointDataArray.Add(FindLockingPointData);
				}
			}
		}
	}
	return LockingPointDataArray;
}

UCancerLockingPointComponent* UCancerLockingTargetComponent::FindLockingPointByAngle(
	TArray<FFindLockingPointInfo> LockingPointDataArray)
{
	for (FFindLockingPointInfo& FindLockingPointData : LockingPointDataArray)
	{
		FRotator FindLookRotator = UKismetMathLibrary::FindLookAtRotation(
			GetOwner()->GetActorLocation(),
			FindLockingPointData.LockingPointComponent->GetComponentLocation());
		//控制夹角
		if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			FRotator ControlDiffRotator = Character->GetControlRotation() - FindLookRotator;
			FRotator ControlDiffRotatorZero = UKismetMathLibrary::NormalizedDeltaRotator(
				FRotator::ZeroRotator, ControlDiffRotator);
			FindLockingPointData.ControllerIncludedAngle =
				FVector2D(ControlDiffRotatorZero.Yaw, ControlDiffRotatorZero.Pitch);
		}
		else
		{
			//角色夹角
			FRotator CharacterDiffRotator = GetOwner()->GetActorRotation() - FindLookRotator;
			FRotator CharacterDiffRotatorZero = UKismetMathLibrary::NormalizedDeltaRotator(
				FRotator::ZeroRotator, CharacterDiffRotator);
			FindLockingPointData.CharacterIncludedAngle =
				FVector2D(CharacterDiffRotatorZero.Yaw, CharacterDiffRotatorZero.Pitch);
			FindLockingPointData.ControllerIncludedAngle = FindLockingPointData.CharacterIncludedAngle;
		}
	}
	//控制器角度排序
	LockingPointDataArray.StableSort([](const FFindLockingPointInfo& A, const FFindLockingPointInfo& B)
	{
		const float AWeight =
			A.ScreenPointDistance * UCancerLockTargetSetting::Get()->LockingConfig.Locking2DDistanceWeight +
			A.Distance * UCancerLockTargetSetting::Get()->LockingConfig.Locking3DDistanceWeight;
		const float BWeight =
			B.ScreenPointDistance * UCancerLockTargetSetting::Get()->LockingConfig.Locking2DDistanceWeight +
			B.Distance * UCancerLockTargetSetting::Get()->LockingConfig.Locking3DDistanceWeight;
		return AWeight < BWeight;
		// return FMath::Abs(A.ControllerIncludedAngle.X) + FMath::Abs(A.ControllerIncludedAngle.Y) <
		// 	FMath::Abs(B.ControllerIncludedAngle.X) + FMath::Abs(B.ControllerIncludedAngle.Y);
	});
	if (LockingPointDataArray.Num() > 0)
	{
		return LockingPointDataArray[0].LockingPointComponent;
	}
	return nullptr;
}

TArray<FFindLockingPointInfo> UCancerLockingTargetComponent::FindLockingPointByX(
	TArray<FFindLockingPointInfo> LockingPointDataArray)
{
	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	for (FFindLockingPointInfo& FindLockingPointData : LockingPointDataArray)
	{
		UGameplayStatics::ProjectWorldToScreen(PlayerController,
											   FindLockingPointData.LockingPointComponent->GetComponentLocation(),
											   FindLockingPointData.Screen2DPoint);
	}
	//控制器角度排序
	LockingPointDataArray.StableSort([](const FFindLockingPointInfo& A, const FFindLockingPointInfo& B)
	{
		return A.Screen2DPoint.X < B.Screen2DPoint.X;
	});

	TArray<FFindLockingPointInfo> LockingPointArray;
	for (FFindLockingPointInfo LockingPointData : LockingPointDataArray)
	{
		LockingPointArray.Add(LockingPointData);
	}
	return LockingPointArray;
}

UCancerLockingPointComponent* UCancerLockingTargetComponent::FindLockingPointByDistance(
	TArray<FFindLockingPointInfo> LockingPointDataArray)
{
	//距离排序
	LockingPointDataArray.StableSort([](const FFindLockingPointInfo& A, const FFindLockingPointInfo& B)
	{
		return A.Distance < B.Distance;
	});
	if (LockingPointDataArray.Num() > 0)
	{
		const float Distance = FVector::Distance(GetOwner()->GetActorLocation(),
												 LockingPointDataArray[0].LockingPointComponent->
																		  GetComponentLocation());
		if (Distance <UCancerLockTargetSetting::Get()->LockingConfig.MaxLockingDistance)
		{
			return LockingPointDataArray[0].LockingPointComponent;
		}
	}
	return nullptr;
}

TArray<FFindLockingPointInfo> UCancerLockingTargetComponent::FindLockingPointByDirection(FVector2D ChangeDirection)
{
	TArray<FFindLockingPointInfo> LockingPointDataArray;
	if (!CurLockingPointComponent) return LockingPointDataArray;
	if (ChangeDirection.IsZero()) return LockingPointDataArray;
	ChangeDirection = UKismetMathLibrary::Normal2D(ChangeDirection);
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(
		FRotator(0, PlayerController->GetControlRotation().Yaw, 0));
	FVector RightVector = UKismetMathLibrary::GetRightVector(
		FRotator(0, PlayerController->GetControlRotation().Yaw, 0));
	FRotator ChangeRotator =
		UKismetMathLibrary::Conv_VectorToRotator(FVector(ChangeDirection.X, ChangeDirection.Y, 0));
	FVector ScreenChangeDirection = UKismetMathLibrary::GreaterGreater_VectorRotator(RightVector, ChangeRotator);
	LockingPointDataArray = FindLockingPointArray();

	bool FindFitTarget = false;
	
	FVector2D SelfScreenPoint = FVector2D();
	UGameplayStatics::ProjectWorldToScreen(PlayerController,
										   GetOwner()->GetActorLocation(),
										   SelfScreenPoint);
	FVector2D CurLockingScreenPoint = FVector2D();
	UGameplayStatics::ProjectWorldToScreen(PlayerController,
	                                       CurLockingPointComponent->GetComponentLocation(),
	                                       CurLockingScreenPoint);
	//2D空间做法
	for (FFindLockingPointInfo& FindLockingPointData : LockingPointDataArray)
	{
		FVector2D ScreenPoint = FVector2D();
		UGameplayStatics::ProjectWorldToScreen(PlayerController,
		                                       FindLockingPointData.LockingPointComponent->GetComponentLocation(),
		                                       ScreenPoint);
		FindLockingPointData.DirectionProjectionDistanceX = FMath::Abs(ScreenPoint.X - CurLockingScreenPoint.X);
		FindLockingPointData.DirectionProjectionDistanceY = FMath::Abs(ScreenPoint.Y - CurLockingScreenPoint.Y);
		FindLockingPointData.DirectionProjectionDistance =
			FindLockingPointData.DirectionProjectionDistanceX +
			FindLockingPointData.DirectionProjectionDistanceY;
		FindLockingPointData.LockingIncludedDistance = FVector2D::Distance(CurLockingScreenPoint, ScreenPoint);

		FVector2D Normal1 = UKismetMathLibrary::Normal2D(ScreenPoint - CurLockingScreenPoint);
		FVector2D Normal2 = UKismetMathLibrary::Normal2D(FVector2D(ChangeDirection.X, ChangeDirection.Y));
		float Dot = UKismetMathLibrary::Dot_VectorVector(FVector(Normal1.X, Normal1.Y, 0),
		                                                 FVector(Normal2.X, Normal2.Y, 0));
		float Acos = UKismetMathLibrary::DegAcos(Dot);
		FindLockingPointData.LockingIncludedAngle = Acos;
		if (FindLockingPointData.LockingIncludedAngle < UCancerLockTargetSetting::Get()->LockingConfig.MaxChangeLockAngle)
		{
			FindFitTarget = true;
		}
	}
	//3D空间做法
	//计算当前目标与每个锁定点，与输入方向上的投影距离
	// FVector LockingLocation = GetOwner()->GetActorLocation();
	FVector LockingLocation = CurLockingPointComponent->GetComponentLocation();
	float Total = FMath::Abs(ChangeDirection.X) + FMath::Abs(ChangeDirection.Y);
	float XScale = FMath::Abs(ChangeDirection.X) / Total;
	float YScale = FMath::Abs(ChangeDirection.Y) / Total;
	XScale = UKismetMathLibrary::FClamp(XScale, 0.1, 1);
	YScale = UKismetMathLibrary::FClamp(YScale, 0.1, 1);
	for (FFindLockingPointInfo& FindLockingPointData : LockingPointDataArray)
	{
		FVector FindLockingPointLocation = FindLockingPointData.LockingPointComponent->GetComponentLocation();
		float SideLength = (FindLockingPointLocation - LockingLocation).Size();
		//跟锁定目标在X轴上的差值
		{
			FVector Normal1 = UKismetMathLibrary::Normal(FindLockingPointLocation - LockingLocation);
			FVector Normal2 = UKismetMathLibrary::Normal(ForwardVector);
			float Dot = UKismetMathLibrary::Dot_VectorVector(Normal1, Normal2);
			FVector ProjectionLocation = SideLength * Dot * Normal2 + LockingLocation;
			FindLockingPointData.DirectionProjectionDistanceX = (FindLockingPointLocation - ProjectionLocation).Size();
			FVector Normal3 = UKismetMathLibrary::Normal(FindLockingPointLocation - ProjectionLocation);
			FVector Normal4 = UKismetMathLibrary::Normal(RightVector);
			float Dot2 = UKismetMathLibrary::Dot_VectorVector(Normal4, Normal3);
			FindLockingPointData.DirectionProjectionDistanceX *= (Dot2 == 0 ? 1 : Dot2);
		}
		//跟锁定目标在Y轴上的差值
		{
			FVector Normal1 = UKismetMathLibrary::Normal(FindLockingPointLocation - LockingLocation);
			FVector Normal2 = UKismetMathLibrary::Normal(RightVector);
			float Dot = UKismetMathLibrary::Dot_VectorVector(Normal1, Normal2);
			FVector ProjectionLocation = SideLength * Dot * Normal2 + LockingLocation;
			FindLockingPointData.DirectionProjectionDistanceY = (FindLockingPointLocation - ProjectionLocation).Size();
			FVector Normal3 = UKismetMathLibrary::Normal(FindLockingPointLocation - ProjectionLocation);
			FVector Normal4 = UKismetMathLibrary::Normal(ForwardVector);
			float Dot2 = UKismetMathLibrary::Dot_VectorVector(Normal4, Normal3);
			FindLockingPointData.DirectionProjectionDistanceY *= (Dot2 == 0 ? 1 : Dot2);
		}
		FindLockingPointData.DirectionProjectionDistance =
			FMath::Abs(FindLockingPointData.DirectionProjectionDistanceX) /** XScale */ +
			FMath::Abs(FindLockingPointData.DirectionProjectionDistanceY) /** YScale*/;

		//与锁定点距离
		{
			FindLockingPointData.LockingIncludedDistance = FVector::Distance(FindLockingPointLocation, LockingLocation);
		}
		//与锁定点夹角
		{
			FVector Normal1 = UKismetMathLibrary::Normal(FindLockingPointLocation - LockingLocation);
			FVector Normal2 = UKismetMathLibrary::Normal(ScreenChangeDirection);
			float Dot = UKismetMathLibrary::Dot_VectorVector(Normal1, Normal2);
			float Acos = UKismetMathLibrary::DegAcos(Dot);
			FindLockingPointData.LockingIncludedAngle = Acos;
			if (Acos < UCancerLockTargetSetting::Get()->LockingConfig.MaxChangeLockAngle)
			{
				FindFitTarget = true;
			}
		}
	}

	if (FindFitTarget)
	{
		for (int i = LockingPointDataArray.Num() - 1; i >= 0; --i)
		{
			if (LockingPointDataArray[i].LockingIncludedAngle >= UCancerLockTargetSetting::Get()->LockingConfig.MaxChangeLockAngle)
			{
				LockingPointDataArray.RemoveAt(i);
				continue;
			}
			float LockDistance = FVector::Distance(LockingPointDataArray[i].LockingPointComponent->GetComponentLocation(), GetOwner()->GetActorLocation());
			float UnLockDistance = FMath::Min(UCancerLockTargetSetting::Get()->LockingConfig.MaxLockingDistance, LockingPointDataArray[i].LockingPointComponent->MaxUnLockingDistance);
			if (LockingPointDataArray[i].LockingIncludedDistance > UCancerLockTargetSetting::Get()->LockingConfig.MaxChangeLockDistance
				|| LockDistance > UnLockDistance)
			{
				LockingPointDataArray.RemoveAt(i);
			}
		}
	}
	if (LockingPointDataArray.Num() == 0) return LockingPointDataArray;
	float MinDirectionProjectionDistance = LockingPointDataArray[0].DirectionProjectionDistance;
	float MaxDirectionProjectionDistance = LockingPointDataArray[0].DirectionProjectionDistance;
	float MinLockingIncludedDistance = LockingPointDataArray[0].LockingIncludedDistance;
	float MaxLockingIncludedDistance = LockingPointDataArray[0].LockingIncludedDistance;
	float MinLockingIncludedAngle = LockingPointDataArray[0].LockingIncludedAngle;
	float MaxLockingIncludedAngle = LockingPointDataArray[0].LockingIncludedAngle;
	for (FFindLockingPointInfo& FindLockingPointData : LockingPointDataArray)
	{
		if (FindLockingPointData.DirectionProjectionDistance < MinDirectionProjectionDistance)
		{
			MinDirectionProjectionDistance = FindLockingPointData.DirectionProjectionDistance;
		}
		if (FindLockingPointData.DirectionProjectionDistance > MaxDirectionProjectionDistance)
		{
			MaxDirectionProjectionDistance = FindLockingPointData.DirectionProjectionDistance;
		}
		if (FindLockingPointData.LockingIncludedDistance < MinLockingIncludedDistance)
		{
			MinLockingIncludedDistance = FindLockingPointData.LockingIncludedDistance;
		}
		if (FindLockingPointData.LockingIncludedDistance > MaxLockingIncludedDistance)
		{
			MaxLockingIncludedDistance = FindLockingPointData.LockingIncludedDistance;
		}
		if (FindLockingPointData.LockingIncludedAngle < MinLockingIncludedAngle)
		{
			MinLockingIncludedAngle = FindLockingPointData.LockingIncludedAngle;
		}
		if (FindLockingPointData.LockingIncludedAngle > MaxLockingIncludedAngle)
		{
			MaxLockingIncludedAngle = FindLockingPointData.LockingIncludedAngle;
		}
	}
	for (FFindLockingPointInfo& FindLockingPointData : LockingPointDataArray)
	{
		FindLockingPointData.DirectionProjectionDistanceWeights = UKismetMathLibrary::MapRangeClamped(
			FindLockingPointData.DirectionProjectionDistance,
			MinDirectionProjectionDistance, MaxDirectionProjectionDistance,
			0, 1);
		FindLockingPointData.LockingIncludedDistanceWeights = UKismetMathLibrary::MapRangeClamped(
			FindLockingPointData.LockingIncludedDistance,
			MinLockingIncludedDistance, MaxLockingIncludedDistance,
			0, 1);
		FindLockingPointData.LockingIncludedAngleWeights = UKismetMathLibrary::MapRangeClamped(
			FindLockingPointData.LockingIncludedAngle,
			MinLockingIncludedAngle, MaxLockingIncludedAngle,
			0, 1);
		FindLockingPointData.TotalWeights =
			FindLockingPointData.DirectionProjectionDistanceWeights +
			// FindLockingPointData.LockingIncludedDistanceWeights +
			FindLockingPointData.LockingIncludedAngleWeights;
	}
	LockingPointDataArray.StableSort([&](const FFindLockingPointInfo& A, const FFindLockingPointInfo& B)
	{
		return SortChangeLockingPointArray(A, B);
		// if (A.TotalWeights == B.TotalWeights)
		// {
		// 	if (A.LockingIncludedAngleWeights == B.LockingIncludedAngleWeights)
		// 	{
		// 		// if (A.LockingIncludedDistanceWeights == B.LockingIncludedDistanceWeights)
		// 		// {
		// 		// 	return A.DirectionProjectionDistanceWeights < B.DirectionProjectionDistanceWeights;
		// 		// }
		// 		return A.LockingIncludedDistanceWeights < B.LockingIncludedDistanceWeights;
		// 	}
		// 	return A.LockingIncludedAngleWeights < B.LockingIncludedAngleWeights;
		// }
		// return A.TotalWeights < B.TotalWeights;
	});

	if (LockingPointDataArray.Num() > 0)
	{
		return LockingPointDataArray;
	}
	return LockingPointDataArray;
}

TArray<FFindLockingPointInfo> UCancerLockingTargetComponent::RankLockingPointByDelegate(
	const TArray<FFindLockingPointInfo>& LockingPointDataArray)
{
	TArray<FFindLockingPointInfo> CopyArray = TArray(LockingPointDataArray);
	CopyArray.StableSort([this](const FFindLockingPointInfo& A, const FFindLockingPointInfo& B)
	{
		return SortLockingPointArray(A, B);
	});
	return CopyArray;
}

bool UCancerLockingTargetComponent::SortLockingPointArray_Implementation(const FFindLockingPointInfo& A,
	const FFindLockingPointInfo& B)
{
	return false;
}

bool UCancerLockingTargetComponent::SortChangeLockingPointArray_Implementation(const FFindLockingPointInfo& A,
	const FFindLockingPointInfo& B)
{
	if (A.TotalWeights == B.TotalWeights)
	{
		if (A.LockingIncludedAngleWeights == B.LockingIncludedAngleWeights)
		{
			// if (A.LockingIncludedDistanceWeights == B.LockingIncludedDistanceWeights)
			// {
			// 	return A.DirectionProjectionDistanceWeights < B.DirectionProjectionDistanceWeights;
			// }
			return A.LockingIncludedDistanceWeights < B.LockingIncludedDistanceWeights;
		}
		return A.LockingIncludedAngleWeights < B.LockingIncludedAngleWeights;
	}
	return A.TotalWeights < B.TotalWeights;
}

FFindLockingPointInfo UCancerLockingTargetComponent::LockingExtraCheck_Implementation(
	const TArray<FFindLockingPointInfo>& LockingPointDataArray)
{
	return LockingPointDataArray.Last();
}

bool UCancerLockingTargetComponent::IsInScreenViewport(const FVector& WorldPosition)
{
	APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0);
	ULocalPlayer* const LocalPlayer = Player ? Player->GetLocalPlayer() : nullptr;
	if (LocalPlayer && LocalPlayer->ViewportClient)
	{
		FSceneViewProjectionData ProjectionData;
		// if (LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, eSSP_FULL, ProjectionData))
		if (LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, ProjectionData))
		{
			FMatrix const ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
			FVector2D ScreenPosition;
			bool bResult = FSceneView::ProjectWorldToScreen(WorldPosition, ProjectionData.GetConstrainedViewRect(),
															ViewProjectionMatrix, ScreenPosition);
			if (bResult &&
				ScreenPosition.X > ProjectionData.GetViewRect().Min.X &&
				ScreenPosition.X < ProjectionData.GetViewRect().Max.X &&
				ScreenPosition.Y > ProjectionData.GetViewRect().Min.Y &&
				ScreenPosition.Y < ProjectionData.GetViewRect().Max.Y)
			{
				return true;
			}
		}
	}
	return false;
}

bool UCancerLockingTargetComponent::IsNoObstruction(UCancerLockingPointComponent* LockingPointComponent)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	FVector Start = PlayerController->PlayerCameraManager->GetCameraLocation();
	FVector End = LockingPointComponent->GetComponentLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = UCancerLockTargetSetting::Get()->DefaultActionObstacleObjectTypes;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(LockingPointComponent->GetOwner());
	ActorsToIgnore.Add(GetOwner());
	FHitResult OutHit;
	if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Start, End, ObjectTypes, false,
														ActorsToIgnore, EDrawDebugTrace::None, OutHit, true,
														FLinearColor::Red, FLinearColor::Green, 0.0f))
	{
		return false;
	}
	return true;
}

bool UCancerLockingTargetComponent::IsCanLock_Implementation(UCancerLockingPointComponent* LockingPointComponent)
{
	auto Owner = LockingPointComponent->GetOwner();
	if (auto ASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
	{
		if (ASC->HasAnyMatchingGameplayTags(LockContainer))
		{
			return false;
		}
	}
	if (LockingPointComponent &&
		LockingPointComponent->IsOpen &&
		Owner &&
		Owner != GetOwner())
	{
		return true;
	}
	return false;
}
