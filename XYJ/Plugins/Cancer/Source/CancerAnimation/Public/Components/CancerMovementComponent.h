#pragma once

#include "CoreMinimal.h"
#include "CancerAnimNativeTags.h"
#include "GameplayTagContainer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Types/AnimInstanceDefinition.h"
#include "CancerMovementComponent.generated.h"

class UCancerMovementNode;
//地面检查
USTRUCT(BlueprintType)
struct FCancerCharacterGroundInfo
{
	GENERATED_BODY()

	FCancerCharacterGroundInfo()
		: LastUpdateFrame(0)
		  , GroundDistance(0.0f)
	{
	}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};

DECLARE_LOG_CATEGORY_EXTERN(CancerMovementLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRagdollStateChangedSignature, bool, bRagdollState);

/*
 *Cancer MovementComponent
 */
UCLASS(ClassGroup=(Cancer), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class CANCERANIMATION_API UCancerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UCancerMovementComponent(const FObjectInitializer& InitializerModule = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//------------ Override
	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;
	virtual FVector ConsumeInputVector() override;
	virtual void PerformMovement(float DeltaTime) override;

	virtual void FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult,
	                       bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult = NULL) const override; //
	virtual bool CanWalkOffLedges() const override; //角色能否走出平台
	//------------

	/*virtual void InitializeComponent() override;
	virtual void SimulateMovement(float DeltaTime) override;
	virtual bool CanAttemptJump() const override;
	virtual bool IsFalling() const override;
	virtual bool IsMovingOnGround() const override;
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	virtual float GetMaxSpeed() const override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void JumpOff(AActor* MovementBaseActor) override;
	
	virtual FVector GetLedgeMove(const FVector& OldLocation, const FVector& Delta,
		const FFindFloorResult& OldFloor) const override;*/

	virtual void PhysicsRotation(float DeltaTime) override;
	virtual FRotator ComputeOrientToMovementRotation(const FRotator& CurrentRotation,
	                                                 float DeltaTime, FRotator& DeltaRotation) const override;
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	//检测平台能否落下
	bool TraceLedges(const FVector& CapsuleLocation) const;

	//DEBUG
	UFUNCTION(BlueprintCallable)
	void DrawDebugMovementInfo();


#pragma region 自定义旋转
	UPROPERTY(BlueprintReadOnly)
	FCustomRotationData CustomRotationData;

	UPROPERTY(BlueprintReadWrite, Category="Rotation")
	bool bUseCustomRotation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotation Rate")
	int GlobalRotationRate = 15;

private:
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	// 更新自定义旋转逻辑：根据当前旋转模式和状态计算角色的自定义旋转
	void UpdateCustomRotation(float DeltaTime);
#pragma endregion


#pragma  region 根运动（速度混合）

public:
	/** 根运动混合比例（0 = 完全当前速度，1 = 完全根运动） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Root Motion")
	float RootMotionBlendAlpha = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Root Motion")
	float RootMotionScale = 1;

	UPROPERTY(BlueprintReadWrite, Category="Root Motion")
	bool bAllowHorizontalRootMotion = true;
	UPROPERTY(BlueprintReadWrite, Category="Root Motion")
	bool bAllowVerticalRootMotion = true;

protected:
	virtual void ApplyRootMotionToVelocity(float DeltaTime) override;

#pragma endregion

#pragma region 状态

public:
	//阻挡移动输入
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Cancer")
	bool bInputBlocked{false};
	//锁定移动模式 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Cancer")
	bool bMovementModeLocked{false};

	//启用边缘检测
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cancer")
	bool bEnableTraceLedgesCheck = true;
	//走出边缘高度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cancer")
	float WalkOffLedgesHeight = 250.f;
	//边缘距离
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cancer")
	float WalkOffLedgesDis = 100.f;

	UPROPERTY()
	FGameplayTag DesiredGaitTag{GaitTag::Run}; //期望
	UPROPERTY()
	FGameplayTag CurGaitTag{GaitTag::Run}; //当前
	UPROPERTY()
	bool bOrientToMovement{true}; //旋转朝向移动方向
	UPROPERTY()
	bool bStand{true}; //站立

	UFUNCTION(BlueprintCallable, Category = "Cancer")
	FGameplayTag GetCurrentGaitTag() const { return CurGaitTag; };
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	bool IsSprint() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	bool IsDesiredSprint() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	bool GetCurrentRotationMode() const { return bOrientToMovement; };
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	bool GetCurrentStand() const { return bStand; };
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	void SetStance(bool NewStand);
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	void SetDesiredGait(FGameplayTag NewTag);
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	void SetOrientToMovement(bool NewRotationMode);
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	bool CanSprint() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	FGameplayTag GetAllowedGait() const;
	UFUNCTION(BlueprintCallable, Category = "Cancer")
	FGameplayTag GetActualGait(const FGameplayTag& AllowTag) const;

	bool bUseCurveRotation = false;
	UFUNCTION(BlueprintCallable, Meta = (BlueprintThreadSafe))
	float GetMovmentPrarameterSpeed(FGameplayTag IntGaitTag) const;
	UFUNCTION(BlueprintCallable, Meta = (BlueprintThreadSafe))
	void SetMovmentPrarameterSpeed(FGameplayTag IntGaitTag, float Speed) const;

	UFUNCTION(BlueprintCallable, Meta = (BlueprintThreadSafe))
	UCancerMovementNode* GetMovementNode(const FName& InName) const;
	void UpdateMovementParameter(); //更新移动参数
#pragma endregion

#pragma  region 折返移动
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BP_Pivot();
	void BP_Pivot_Implementation();
#pragma endregion

#pragma  region 移动参数设置
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Instanced, Category="CancerMovementComponent")
	TObjectPtr<UCancerMovementNode> MovementParameter;
#pragma endregion

#pragma  region 计算地面距离
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe), Category = "CancerMovementComponent")
	const FCancerCharacterGroundInfo& GetGroundInfo();

	UPROPERTY()
	FCancerCharacterGroundInfo CachedGroundInfo;
#pragma endregion

#pragma region  移动参数计算
	UPROPERTY(BlueprintReadWrite)
	FRotator LastVelocityRotation;
	UPROPERTY(BlueprintReadWrite)
	FRotator LastMovementInputRotation;
	UPROPERTY(BlueprintReadWrite)
	bool bIsMove = false;
	UPROPERTY(BlueprintReadWrite)
	bool HasMovementInput = false;
	UPROPERTY(BlueprintReadWrite)
	FVector VelocityAcceleration;
	UPROPERTY(BlueprintReadWrite)
	float CurrentAimRotationYaw;
	UPROPERTY(BlueprintReadWrite)
	float CurrentRotationScale = 1.0f;
	UPROPERTY()
	float CurrentSpeed2D;
	UPROPERTY()
	FVector PreviousVelocity;
	UPROPERTY()
	float PreviousRotationYaw;
	void SetEssentialValues(float DeltaTime);
	void SetPreviousValues();
#pragma endregion


#pragma  region 布娃娃状态

	UPROPERTY(BlueprintReadOnly, Category="Cancer|Ragdoll")
	bool bIsRagdollState{false};
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Ragdoll")
	FVector LastRagdollVelocity;
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Ragdoll")
	bool bRagdollFaceUp;
	UPROPERTY(BlueprintReadOnly, Category="Cancer|Ragdoll")
	bool bRagdollOnGround;
	UPROPERTY(BlueprintAssignable, Category = "Cancer|Ragdoll")
	FRagdollStateChangedSignature RagdollStateChangedDelegate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Cancer|Ragdoll")
	TSoftObjectPtr<UAnimMontage> GetUp_F;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Cancer|Ragdoll")
	TSoftObjectPtr<UAnimMontage> GetUp_B;

	UAnimMontage* GetRagdollMontage(bool InRagdollFaceUp) const;

	//缓存的布娃娃状态快照名
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FName GetRagdollPoseSnapshotName() const;
	//开始
	UFUNCTION(BlueprintCallable)
	virtual void RagdollStart();
	//结束
	UFUNCTION(BlueprintCallable)
	virtual void RagdollEnd();
	//更新
	UFUNCTION(BlueprintCallable)
	void RagdollUpdate(float DeltaTime);
	//更新胶囊体的位置
	void SetActorLocationDuringRagdoll(float DeltaTime);
#pragma endregion
};
