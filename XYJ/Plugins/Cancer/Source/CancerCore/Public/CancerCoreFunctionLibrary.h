#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CancerCoreFunctionLibrary.generated.h"


class UCancerCombatData;
class UCancerHeroExtensionComponent;

USTRUCT(BlueprintType)
struct FCancerQueryMatch
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagQuery TagQuery;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="反转条件")
	bool bReverse = false;
};

/**
 * 
 */
UCLASS()
class CANCERCORE_API UCancerCoreFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
#pragma  region 获取战斗网格
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	static USkeletalMeshComponent* GetCombatMesh(AActor* Actor);
#pragma endregion

#pragma  region 获取战斗动画实例
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	static UAnimInstance* GetCombatAnimInstance(AActor* Actor);
#pragma endregion

#pragma  region 获取战斗前向箭头
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	static USceneComponent* GetCombatForwardReference(AActor* Actor);
#pragma endregion

	/** Returns the soft object reference associated with a Primary Asset Id, this works even if the asset is not loaded */
	UFUNCTION(BlueprintPure, Category = "AssetManager", meta=(DeterminesOutputType=ExpectedAssetType))
	static TSoftObjectPtr<UObject> GetTypedSoftObjectReferenceFromPrimaryAssetId(
		FPrimaryAssetId PrimaryAssetId, TSubclassOf<UObject> ExpectedAssetType);

	// Sets ParameterName to ParameterValue on all sections of all mesh components found on the TargetActor
	UFUNCTION(BlueprintCallable, Category = "Rendering|Material", meta=(DefaultToSelf="TargetActor"))
	static void SetScalarParameterValueOnAllMeshComponents(AActor* TargetActor, const FName ParameterName,
	                                                       const float ParameterValue, bool bIncludeChildActors = true);

	// Sets ParameterName to ParameterValue on all sections of all mesh components found on the TargetActor
	UFUNCTION(BlueprintCallable, Category = "Rendering|Material", meta=(DefaultToSelf="TargetActor"))
	static void SetVectorParameterValueOnAllMeshComponents(AActor* TargetActor, const FName ParameterName,
	                                                       const FVector ParameterValue,
	                                                       bool bIncludeChildActors = true);

	// Sets ParameterName to ParameterValue on all sections of all mesh components found on the TargetActor
	UFUNCTION(BlueprintCallable, Category = "Rendering|Material", meta=(DefaultToSelf="TargetActor"))
	static void SetColorParameterValueOnAllMeshComponents(AActor* TargetActor, const FName ParameterName,
	                                                      const FLinearColor ParameterValue,
	                                                      bool bIncludeChildActors = true);

	// Gets all the components that inherit from the given class
	UFUNCTION(BlueprintCallable, Category = "Actor",
		meta=(DefaultToSelf="TargetActor", ComponentClass="/Script/Engine.ActorComponent", DeterminesOutputType=
			"ComponentClass"))
	static TArray<UActorComponent*> FindComponentsByClass(AActor* TargetActor,
	                                                      TSubclassOf<UActorComponent> ComponentClass,
	                                                      bool bIncludeChildActors = true);

	// Get a montage from an array
	UFUNCTION(BlueprintCallable, Category = "Animation")
	static UAnimMontage* GetRandomAnim(TArray<UAnimMontage*> AnimArray);

	// Get a montage from an array , Diff with last time played animation
	UFUNCTION(BlueprintCallable, Category = "Animation")
	static UAnimMontage* GetRandomAnimDiff(TArray<UAnimMontage*> AnimArray, UAnimMontage* LastTimeMontage);


	UFUNCTION(BlueprintCallable, Category = "Cancore")
	static bool MatchAllQuery(const TArray<FCancerQueryMatch>& QueryMatches, const FGameplayTagContainer& TagContainer);
};
