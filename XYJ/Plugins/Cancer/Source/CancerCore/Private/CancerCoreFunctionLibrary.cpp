#include "CancerCoreFunctionLibrary.h"

#include "Engine/AssetManager.h"
#include "Engine/AssetManagerTypes.h"
#include "Interfaces/CancerCoreSystemInterface.h"

USkeletalMeshComponent* UCancerCoreFunctionLibrary::GetCombatMesh(AActor* Actor)
{
	if (Actor == nullptr || !Actor->Implements<UCancerCoreSystemInterface>()) return nullptr;
	return ICancerCoreSystemInterface::Execute_GetCombatMesh(Actor);
}

UAnimInstance* UCancerCoreFunctionLibrary::GetCombatAnimInstance(AActor* Actor)
{
	if (Actor == nullptr || !Actor->Implements<UCancerCoreSystemInterface>())return nullptr;
	return ICancerCoreSystemInterface::Execute_GetCombatAnimInstance(Actor);
}

USceneComponent* UCancerCoreFunctionLibrary::GetCombatForwardReference(AActor* Actor)
{
	if (Actor == nullptr || !Actor->Implements<UCancerCoreSystemInterface>())return nullptr;
	return ICancerCoreSystemInterface::Execute_GetCombatForwardReference(Actor);
}

TSoftObjectPtr<UObject> UCancerCoreFunctionLibrary::GetTypedSoftObjectReferenceFromPrimaryAssetId(
	FPrimaryAssetId PrimaryAssetId, TSubclassOf<UObject> ExpectedAssetType)
{
	if (UAssetManager* Manager = UAssetManager::GetIfInitialized())
	{
		FPrimaryAssetTypeInfo Info;
		if (Manager->GetPrimaryAssetTypeInfo(PrimaryAssetId.PrimaryAssetType, Info) && !Info.bHasBlueprintClasses)
		{
			if (UClass* AssetClass = Info.AssetBaseClassLoaded)
			{
				if ((ExpectedAssetType == nullptr) || !AssetClass->IsChildOf(ExpectedAssetType))
				{
					return nullptr;
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning,
				       TEXT(
					       "GetTypedSoftObjectReferenceFromPrimaryAssetId(%s, %s) - AssetBaseClassLoaded was unset so we couldn't validate it, returning null"
				       ),
				       *PrimaryAssetId.ToString(),
				       *GetPathNameSafe(*ExpectedAssetType));
			}

			return TSoftObjectPtr<UObject>(Manager->GetPrimaryAssetPath(PrimaryAssetId));
		}
	}
	return nullptr;
}

void UCancerCoreFunctionLibrary::SetScalarParameterValueOnAllMeshComponents(AActor* TargetActor,
                                                                            const FName ParameterName,
                                                                            const float ParameterValue,
                                                                            bool bIncludeChildActors)
{
	if (TargetActor != nullptr)
	{
		TargetActor->ForEachComponent<UMeshComponent>(bIncludeChildActors, [=](UMeshComponent* InComponent)
		{
			InComponent->SetScalarParameterValueOnMaterials(ParameterName, ParameterValue);
		});
	}
}

void UCancerCoreFunctionLibrary::SetVectorParameterValueOnAllMeshComponents(AActor* TargetActor,
                                                                            const FName ParameterName,
                                                                            const FVector ParameterValue,
                                                                            bool bIncludeChildActors)
{
	if (TargetActor != nullptr)
	{
		TargetActor->ForEachComponent<UMeshComponent>(bIncludeChildActors, [=](UMeshComponent* InComponent)
		{
			InComponent->SetVectorParameterValueOnMaterials(ParameterName, ParameterValue);
		});
	}
}

void UCancerCoreFunctionLibrary::SetColorParameterValueOnAllMeshComponents(AActor* TargetActor,
                                                                           const FName ParameterName,
                                                                           const FLinearColor ParameterValue,
                                                                           bool bIncludeChildActors)
{
	SetVectorParameterValueOnAllMeshComponents(TargetActor, ParameterName, FVector(ParameterValue),
	                                           bIncludeChildActors);
}

TArray<UActorComponent*> UCancerCoreFunctionLibrary::FindComponentsByClass(AActor* TargetActor,
                                                                           TSubclassOf<UActorComponent> ComponentClass,
                                                                           bool bIncludeChildActors)
{
	TArray<UActorComponent*> Components;
	if (TargetActor != nullptr)
	{
		TargetActor->GetComponents(ComponentClass, /*out*/ Components, bIncludeChildActors);
	}
	return MoveTemp(Components);
}

UAnimMontage* UCancerCoreFunctionLibrary::GetRandomAnim(TArray<UAnimMontage*> AnimArray)
{
	if (AnimArray.IsEmpty()) return nullptr;
	if (AnimArray.Num() == 1) return AnimArray[0];

	int32 length = AnimArray.Num();
	length = FMath::RandRange(0, length - 1);
	return AnimArray[length];
}

UAnimMontage* UCancerCoreFunctionLibrary::GetRandomAnimDiff(TArray<UAnimMontage*> AnimArray,
                                                            UAnimMontage* LastTimeMontage)
{
	if (AnimArray.IsEmpty()) return nullptr;
	if (AnimArray.Num() == 1) return AnimArray[0];

	for (int i = AnimArray.Num() - 1; i > 0; --i)
	{
		if (AnimArray[i] == LastTimeMontage)
		{
			AnimArray.RemoveAt(i);
		}
	}
	int32 length = AnimArray.Num();

	if (length == 0) return LastTimeMontage;
	if (length == 1) return AnimArray[0];

	length = FMath::RandRange(0, length - 1);
	return AnimArray[length];
}

bool UCancerCoreFunctionLibrary::MatchAllQuery(const TArray<FCancerQueryMatch>& QueryMatches,
                                               const FGameplayTagContainer& TagContainer)
{
	bool bResult = true;
	for (auto Query : QueryMatches)
	{
		if (Query.TagQuery.Matches(TagContainer))
		{
			if (Query.bReverse)
			{
				bResult = false;
				break;
			}
		}
		else
		{
			if (!Query.bReverse)
			{
				bResult = false;
				break;
			}
		}
	}
	return bResult;
}
