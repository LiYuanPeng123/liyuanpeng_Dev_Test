#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CancerIndicatorSubsystem.generated.h"

class UCancerIndicatorSourceComponent;






DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIndicatorChange, UCancerIndicatorSourceComponent*, IndicatorSourceComponent,bool,bAdd);
/**
 * 
 */
UCLASS()
class CANCERINDICATOR_API UCancerIndicatorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnIndicatorChange OnIndicatorChange;
	
	UFUNCTION(BlueprintCallable)
	void AddIndicator(UCancerIndicatorSourceComponent* SourceIndicatorComponent);

	UFUNCTION(BlueprintCallable)
	void RemoveIndicator(UCancerIndicatorSourceComponent* SourceIndicatorComponent);

	UFUNCTION(BlueprintCallable)
	TArray<UCancerIndicatorSourceComponent*> GetIndicatorComponents();

	UPROPERTY()
	TArray<UCancerIndicatorSourceComponent*> IndicatorSourceArray;
};
