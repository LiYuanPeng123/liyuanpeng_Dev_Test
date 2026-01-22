#pragma once

#include "CoreMinimal.h"
#include "CancerGameplay/Public/CancerControllerComponent.h"
#include "CancerIndicatorManagerComponent.generated.h"

#define UE_API CANCERINDICATOR_API

class UCancerIndicatorSourceComponent;
class AController;
class UIndicatorDescriptor;
class UObject;
struct FFrame;

UCLASS(ClassGroup=(Cancer), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class UCancerIndicatorManagerComponent : public UCancerControllerComponent
{
	GENERATED_BODY()

public:
	UE_API UCancerIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);

	static UE_API UCancerIndicatorManagerComponent* GetComponent(AController* Controller);


	UFUNCTION(BlueprintCallable, Category = Indicator)
	void HandleIndicatorComponent(UCancerIndicatorSourceComponent* IndicatorSourceComponent
		,bool bAdd);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// 添加指示器：注册并触发 OnIndicatorAdded
	UFUNCTION(BlueprintCallable, Category = Indicator)
	UE_API void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	// 移除指示器：触发 OnIndicatorRemoved 并从列表移除
	UFUNCTION(BlueprintCallable, Category = Indicator)
	UE_API void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	// 事件：指示器添加/移除
	DECLARE_EVENT_OneParam(ULyraIndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* Descriptor)

	FIndicatorEvent OnIndicatorAdded;
	FIndicatorEvent OnIndicatorRemoved;

	// 读取当前所有指示器
	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }

private:
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators; // 指示器列表
};

#undef UE_API
