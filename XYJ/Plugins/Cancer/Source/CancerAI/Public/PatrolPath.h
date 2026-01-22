#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class CANCERAI_API APatrolPath : public AActor
{
	GENERATED_BODY()

public:
	APatrolPath();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Cancer)
	class USplineComponent* SplineComp;

public:
	UPROPERTY(EditAnywhere, Category = Cancer,DisplayName="路径标签")
	FGameplayTag PathTag;
	UFUNCTION(BlueprintPure, Category = Cancer)
	FORCEINLINE class USplineComponent* GetSplineComponent() const { return SplineComp; }
	
	UFUNCTION(BlueprintCallable, Category = Cancer)
	void AddSplinePoint(const FVector& worldPos);

	UFUNCTION(BlueprintCallable, Category = Cancer)
	void SetSplinePoints(const TArray<FVector>& worldPos);
};
