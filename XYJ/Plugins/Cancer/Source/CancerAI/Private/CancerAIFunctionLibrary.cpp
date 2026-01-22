// Fill out your copyright notice in the Description page of Project Settings.


#include "CancerAIFunctionLibrary.h"

#include "AIPatrolComponent.h"
#include "CancerAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/CancerPickupActor.h"
#include "GameFramework/Character.h"
#include "Interfaces/ICancerPickupable.h"

class UAIPatrolComponent* UCancerAIFunctionLibrary::K2_GetAIPatrolComponent(AActor* Actor, bool& bIsValid,
                                                                            TSubclassOf<UAIPatrolComponent> OutputClass)
{
	bIsValid=false;
	if (auto ReturnValue = GetAIPatrolComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UAIPatrolComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

bool UCancerAIFunctionLibrary::StopAIBehaviorTree(AActor* AIControlledActor)
{

	auto AICharacter = Cast<ACharacter>(AIControlledActor);
	if (!IsValid(AICharacter))
	{
		return false;
	}

	// 获取AI控制器
	AAIController* AIController = Cast<AAIController>(AICharacter->GetController());
	if (!AIController)
	{
		return false;
	}

	// 获取行为树组件
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp =  Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent());
	
	if (!IsValid(BehaviorTreeComp))
	{
		return false;
	}

	// 停止行为树
	BehaviorTreeComp->StopTree();
	return true;
}

bool UCancerAIFunctionLibrary::StartAIBehaviorTree(AActor* AIControlledActor)
{
	auto AICharacter = Cast<ACharacter>(AIControlledActor);
	if (!IsValid(AICharacter))
	{
		return false;
	}

	// 获取AI控制器
	AAIController* AIController = Cast<AAIController>(AICharacter->GetController());
	if (!AIController)
	{
		return false;
	}

	// 获取行为树组件
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp =  Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent());
	
	if (!IsValid(BehaviorTreeComp))
	{
		return false;
	}

	// 开始行为树
	BehaviorTreeComp->RestartTree();
	return true;
}

AActor* UCancerAIFunctionLibrary::FindWeaponActor(APawn* ControlledPawn, FGameplayTag WeaponTag, float MaxDistance, bool ShowDebugRange)
{
	AActor* PickupWeaponTarget = nullptr;
	if (ControlledPawn)
	{
		if (UWorld* World = ControlledPawn->GetWorld())
		{
			// 查询
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(ControlledPawn);
			TArray<FHitResult> HitResults;
			if (ShowDebugRange)
			{	
				DrawDebugSphere(World,ControlledPawn->GetActorLocation(),MaxDistance,20,FColor::Red,false,5.f,0,10.f);
			}
			World->SweepMultiByChannel(HitResults,
				ControlledPawn->GetActorLocation(),
				ControlledPawn->GetActorLocation()+ FVector(0.f,0.f,1.f),
				FRotator().Quaternion(),ECollisionChannel::
				ECC_WorldDynamic,FCollisionShape::MakeSphere(MaxDistance),
				QueryParams,
				FCollisionResponseParams::DefaultResponseParam
				);
			if (HitResults.IsEmpty()) return nullptr;
			
			// 过滤
			float MinDistance = FLT_MAX;
			for (const FHitResult& Hit : HitResults)
			{
				if (!Hit.GetActor()->IsA(ACancerPickupActor::StaticClass())) continue;
				if (Hit.GetActor()->Implements<UCancerPickupable>())
				{
					FGameplayTag InteractionItemTag = ICancerPickupable::Execute_GetPickupItemTag(Hit.GetActor());
					if (InteractionItemTag.MatchesTag(WeaponTag))
					{
						float Distance = Hit.GetActor()->GetDistanceTo(ControlledPawn);
						if (Distance < MinDistance)
						{
							PickupWeaponTarget = Hit.GetActor();
							MinDistance = Distance;
						}
					}
				}
			}
		}
	}
	return PickupWeaponTarget;
}
