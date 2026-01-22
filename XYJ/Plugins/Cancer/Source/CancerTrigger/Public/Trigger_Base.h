#pragma once

#include "CoreMinimal.h"
#include "Trigger_Base.generated.h"

UCLASS()
class CANCERTRIGGER_API UItemTrigger_Base : public UDataAsset
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="使用加载框")
	bool bUseLoadShape = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, DisplayName="加载器类型",
		meta=(EditConditionHides, EditCondition="bUseLoadShape"))
	class UTriggerShape_Base* LoadShape;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, DisplayName="触发器类型")
	class UTriggerShape_Base* TriggerShape;
};

USTRUCT(BlueprintType)
struct FTriggerItemData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="", DisplayName="配置数据类型")
	TSubclassOf<class UItemTrigger_Base> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="", DisplayName="配置数据资产")
	TObjectPtr<UItemTrigger_Base> Item;
};

UCLASS()
class CANCERTRIGGER_API ATrigger_Base : public AActor
{
	GENERATED_BODY()

public:
	ATrigger_Base(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable)
	void BeginComponentOverlap();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cancer|TriggerData")
	TObjectPtr<UItemTrigger_Base> TriggerData;
	
	//已经触发的Actor
	UPROPERTY()
	TArray<AActor*> TriggerActorArray;

	//最后触发Actor
	UPROPERTY(BlueprintReadOnly)
	AActor* LastTriggerActor;

	//触发碰撞组件
	UPROPERTY(BlueprintReadOnly)
	UPrimitiveComponent* CollisionTriggerComponent;

	//加载碰撞组件
	UPROPERTY(BlueprintReadOnly)
	UPrimitiveComponent* CollisionLoadComponent;

	//已经进入触发框的Actor
	UPROPERTY(BlueprintReadOnly, Transient)
	TSet<AActor*> EnterTriggerActors;

	//已经进入加载框的Actor
	UPROPERTY(BlueprintReadOnly, Transient)
	TSet<AActor*> EnterLoadActors;

protected:
	UFUNCTION()
	virtual void BeginTriggerComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                          const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndTriggerComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void BeginLoadComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                       const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndLoadComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	//当有组件进入时触发
	//调用流程: CollisionTriggerComponent Overlap -> BeginTriggerComponentOverlap -> OnTriggerComponentEnter
	//返回 true 表示允许触发 OnTriggerEnter，返回 false 则拦截
	UFUNCTION(BlueprintNativeEvent)
	bool OnTriggerComponentEnter(AActor* OtherActor, UPrimitiveComponent* OtherComp);

	//当有组件退出时触发
	//调用流程: CollisionTriggerComponent EndOverlap -> EndTriggerComponentOverlap -> OnTriggerComponentExit
	//返回 true 表示允许触发 OnTriggerExit，返回 false 则拦截
	UFUNCTION(BlueprintNativeEvent)
	bool OnTriggerComponentExit(AActor* OtherActor, UPrimitiveComponent* OtherComp);

	//真正的进入触发逻辑
	//调用流程: OnTriggerComponentEnter 返回 true -> OnTriggerEnter
	UFUNCTION(BlueprintNativeEvent)
	void OnTriggerEnter(AActor* OtherActor, UPrimitiveComponent* OtherComp);

	//真正的退出触发逻辑
	//调用流程: OnTriggerComponentExit 返回 true -> OnTriggerExit
	UFUNCTION(BlueprintNativeEvent)
	void OnTriggerExit(AActor* OtherActor, UPrimitiveComponent* OtherComp);


	//当有组件进入加载框（显示范围）时触发
	//调用流程: CollisionLoadComponent Overlap -> BeginLoadComponentOverlap -> OnShowTriggerEnter
	UFUNCTION(BlueprintNativeEvent)
	void OnShowTriggerEnter(AActor* OtherActor, UPrimitiveComponent* OtherComp);

	//当有组件退出加载框（显示范围）时触发
	//调用流程: CollisionLoadComponent EndOverlap -> EndLoadComponentOverlap -> OnShowTriggerExit
	UFUNCTION(BlueprintNativeEvent)
	void OnShowTriggerExit(AActor* OtherActor, UPrimitiveComponent* OtherComp);
};
