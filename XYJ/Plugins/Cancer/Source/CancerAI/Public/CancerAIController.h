#pragma once

#include "CoreMinimal.h"
#include "CancerBaseAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKey.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionConfigAsset.h"
#include "CancerAIController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIStateChanged, const EAIState, AIState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAICombatStateChanged, const EAICombatState, AICombatState);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLockTarget,APawn*,ControlPawn ,AActor*,TargetActor);
UCLASS()
class CANCERAI_API ACancerAIController : public ACancerBaseAIController
{
	GENERATED_BODY()

public:
	ACancerAIController();

	UFUNCTION(BlueprintPure, Category = "Cancer| SpawnPosition")
	FVector GetSpawnActorLocation() const { return SpawnActorLocation; }

	TObjectPtr<class UAIPatrolComponent> TryGetPatrolComp() const;
	UFUNCTION(BlueprintCallable)
	void SetBehaviorTree(UBehaviorTree* InBehaviorTree);

	UPROPERTY(BlueprintAssignable)
	FOnLockTarget OnLockTarget;

	void StartBehaviorTree();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type reason) override;

	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cancer")
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	UPROPERTY(BlueprintReadOnly, Category = Cancer)
	TObjectPtr<class ACharacter> CharacterOwned;
	UPROPERTY(BlueprintReadOnly, Category = Cancer)
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComponent;
	UPROPERTY(BlueprintReadOnly, Category = Cancer)
	TObjectPtr<class UBlackboardComponent> BlackboardComponent;

	UFUNCTION(BlueprintCallable, Category = "Cancer | Patrol")
	float GetPathDistanceFromTarget();

public:
#pragma region 设置获取黑板键值

	void SetTargetActorBK(AActor* target);
	AActor* GetTargetActorBK() const;
	void SetAIStateBK(EAIState state);
	EAIState GetAIStateBK() const;
	void SetAICombatStateBK(EAICombatState state);
	EAICombatState GetAICombatStateBK() const;
#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "Cancer | Target")
	void SetTarget(AActor* target);
	UFUNCTION(BlueprintPure, Category = "Cancer | Target")
	AActor* GetTarget() const;
	UFUNCTION(BlueprintPure, Category = "Cancer | Target")
	bool HasTarget() const;

	// 请求更换一个目标：从当前感知组件中选择最近的目标，若成功返回true
	UFUNCTION(BlueprintCallable, Category = "Cancer | Target")
	bool RequestAnotherTarget();

	// 超过该距离判定为丢失目标
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cancer | Combat")
	float LoseTargetDistance = 3500.f;

	// 感知不可见持续超过该秒数也判定为丢失（可选）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cancer | Combat")
	float LoseTargetTimeout = 2.0f;

	// Active perception config set by AAICreator (optional); used to filter perceived targets
	UFUNCTION(BlueprintCallable, Category="Cancer|Perception")
	void SetActivePerceptionConfig(UAIPerceptionConfigAsset* InConfig) { ActivePerceptionConfig = InConfig; }

private:
	virtual void OnPossess(APawn* _possPawn) override;

	virtual void OnUnPossess() override;

	UFUNCTION()
	void HandleOnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UFUNCTION()
	void HandleOnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
	
	UFUNCTION()
	void HandleOnTargetPerceptionForgotten( AActor* Actor);
	
	UFUNCTION()
	void HandleOnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);

	// 出生点位置
	FVector SpawnActorLocation;

	// 最近一次成功感知目标的时间戳
	float LastSensedTime = 0.f;

	UPROPERTY()
	TObjectPtr<class UAIPatrolComponent> patrolComp;

	// 黑板Key缓存
	struct FBlackboard::FKey
		targetActorKey, //目标
		targetActorDistanceKey, //目标距离
		targetPointLocationKey, //目标位置
		aiStateKey, //AI状态
		aiCombatStateKey, //AI战斗状态
		isPausedKey, //是否暂停
		homeDistanceKey, //离初始点的距离
		pathPointKey; //前往的路点位置


	UPROPERTY(Transient)
	TObjectPtr<UAIPerceptionConfigAsset> ActivePerceptionConfig;
};
