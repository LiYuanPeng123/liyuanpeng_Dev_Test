#pragma once

#include "CoreMinimal.h"
#include "Types/FActorPool.h"
#include "Types/FPoolableActorInfo.h"
#include "CancerActorPoolComponent.generated.h"


/*
 * 对象池组件（管理Actor的复用）
 * - 预生成指定类的Actor并保持“未激活”状态
 * - 借出时切换为“保留/激活”，归还后重置为“未激活”
 * - 减少频繁Spawn/Destroy的性能开销
 */
UCLASS(ClassGroup=(Cancer), Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class UCancerActorPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCancerActorPoolComponent();
	// -- Begin Actor Component implementation
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// -- End Actor Component implementation


	UFUNCTION(BlueprintCallable, Category = "Actor Pool Component",
		meta = (DeterminesOutputType = ActorClass))
	virtual AActor* GetActorFromPool(TSubclassOf<AActor> ActorClass, bool bActivate = false);

	UFUNCTION(BlueprintCallable, Category = "Actor Pool Component")
	virtual AActor* GetActorByTagFromPool(const FGameplayTag& InTag, bool bActivate = false);

	template <typename ActorT = AActor>
	ActorT* GetTypedActorFromPool(const TSubclassOf<ActorT>& ActorClass)
	{
		return Cast<ActorT>(GetActorFromPool(ActorClass));
	}

	// 由池组件生成Actor并初始化为“未激活”状态
	UFUNCTION(BlueprintCallable, Category = "Actor Pool Component")
	AActor* SpawnActor(
		UPARAM(meta = (MustImplement = "/Script/CancerActorPool.PoolableActorInterface"))
		const TSubclassOf<AActor>& ActorClass,
		const FGuid& PoolableActorId);

	void SetPoolableActors(const TArray<FPoolableActorInfo>& InPoolableActors)
	{
		PoolableActors = InPoolableActors;
	};

protected:
	// 生成时的碰撞处理策略
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actor Pool Component")
	ESpawnActorCollisionHandlingMethod CollisionHandlingMethod;


	// 需要预生成的池化Actor及其数量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actor Pool Component")
	TArray<FPoolableActorInfo> PoolableActors;

	// 初始化对象池（根据PoolableActors配置创建实例）
	virtual void InitializePool();


	// 初始化新生成Actor为“未激活”可复用状态
	virtual void InitializeActor(AActor* NewActor);


	// 拥有者是否为本地控制（用于仅对拥有者可见的Actor跳过生成）
	bool OwnerIsLocallyControlled() const;


	// 用于生成参数中的Instigator
	virtual APawn* GetInstigator() const;


	// 构建Spawn参数（名称、拥有者、碰撞处理、延迟构造等）
	virtual FActorSpawnParameters BuildSpawnParameters(const TSubclassOf<AActor>& ActorClass,
	                                                   const FGuid& PoolableActorId) const;


	// 构建池内唯一名称：类名 + GUID
	virtual FString BuildActorName(const TSubclassOf<AActor>& ActorClass, const FGuid& PoolableActorId) const;


	// 初始变换（默认单位矩阵，可按需重载）
	virtual FTransform BuildInitialTransform(const AActor* NewActor);

private:
	// 对象池条目集合（本地管理）
	UPROPERTY()
	FActorPoolList Pool;
};
