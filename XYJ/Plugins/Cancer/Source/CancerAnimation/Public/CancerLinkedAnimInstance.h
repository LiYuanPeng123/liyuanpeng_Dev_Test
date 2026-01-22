#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Types/AnimInstanceDefinition.h"
#include "CancerLinkedAnimInstance.generated.h"

class UCancerLinkedAnimInstance;
struct FAnimUpdateContext;
struct FAnimNodeReference;
class UCancerMovementComponent;
class UCancerLocomotionData;
class UCancerAnimInstance;
class UChooserTable;

USTRUCT(BlueprintType)
struct FCurrentAnimData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float EntryAccelerationAngle{}; // 进入动画状态时的加速度角度
	UPROPERTY(BlueprintReadOnly)
	float EntryVelocityAngle{}; // 进入动画状态时的速度角度
	UPROPERTY(BlueprintReadOnly)
	float EntryRotationYaw{}; // 进入动画状态时的Yaw值

	UPROPERTY(BlueprintReadOnly)
	ECancerGait EntryGait{ECancerGait::Walk}; // 进入动画状态时的步态

	UPROPERTY(BlueprintReadOnly)
	ECancerDirection_4 EntryVelocityDirection{}; // 进入动画状态时的基本方向4

	UPROPERTY(BlueprintReadOnly)
	ECancerDirection_8 EntryVelocityDirection_8{}; // 进入动画状态时的基本方向 8

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> AnimSequence{}; // 当前播放的动画序列资源引用

	UPROPERTY(BlueprintReadOnly)
	float FirstStepAlpha{}; // 第一步混合权重(动画开始的前几帧)
	UPROPERTY(BlueprintReadOnly)
	float StrideWarpingAlpha{}; // 步幅扭曲混合Alpha值，控制步幅调整强度
	UPROPERTY(BlueprintReadOnly)
	float OrientationWarpingAlpha{}; // 方向扭曲混合Alpha值，控制方向调整强度
	UPROPERTY(BlueprintReadOnly)
	float TargetAngle{}; // 目标角度
	UPROPERTY(BlueprintReadOnly)
	float CurrentAccelerationDirection{}; // 当前加速度角度


	UPROPERTY(BlueprintReadOnly)
	float DesiredRotationYaw{}; // 期望的Yaw值
	UPROPERTY(BlueprintReadOnly)
	float AnimStartPosition{}; // 动画开始播放的位置（时间）
};


USTRUCT(BlueprintType)
struct FCancerDirection_6
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly)
    ECancerGait Gait;
    UPROPERTY(BlueprintReadOnly)
    ECancerDirection_6 Direction;
    FCancerDirection_6()
    {
    }

    FCancerDirection_6(ECancerGait InGait, ECancerDirection_6 InDir)
    {
        Gait = InGait;
        Direction = InDir;
    }
};

/**
 * Cancer LinkedAnimInstance
 */
UCLASS()
class CANCERANIMATION_API UCancerLinkedAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<UCancerAnimInstance> Parent;
	UPROPERTY(BlueprintReadOnly, Transient)
	TObjectPtr<ACharacter> Character;

public:
	UCancerLinkedAnimInstance();

	virtual void PostLoad() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;

protected:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;

	//Idle
	UPROPERTY(BlueprintReadOnly)
	FCurrentAnimData IdleAnimData;
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_IdleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);


	//Start
	UPROPERTY(BlueprintReadOnly)
	FCurrentAnimData StartAnimData;
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_StartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Update_StartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	//Cycle
	UPROPERTY(BlueprintReadOnly)
	FCurrentAnimData CycleAnimData;
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_CycleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	/*UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Update_CycleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);*/

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void UpdateMove_F(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void UpdateMove_FL(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void UpdateMove_FR(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void UpdateMove_B(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void UpdateMove_BL(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void UpdateMove_BR(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	//Stop
	UPROPERTY(BlueprintReadOnly)
	FCurrentAnimData StopAnimData;
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_StopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Update_StopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	//Pivot
	UPROPERTY(BlueprintReadOnly)
	FCurrentAnimData PivotAnimData;
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_PivotAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Update_PivotAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_PivotTestAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Update_PivotTestAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	//Post
	UPROPERTY(BlueprintReadOnly)
	FCurrentAnimData PostAnimData;
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_PostAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Update_PostAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	//Cross
	UPROPERTY(BlueprintReadOnly)
	FCurrentAnimData CrossTurnAnimData;
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_CrossTurnAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Update_CrossTurnAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	//Jump

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_JumpStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_JumpStartLoopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_FallLandAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_Double(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);


	//TurnInPlace
	UPROPERTY(BlueprintReadOnly)
	FCurrentAnimData TurnInPlaceAnimData;
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_TurnInPlace(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Update_TurnInPlace(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UPROPERTY(BlueprintReadOnly)
	FCurrentAnimData TurnInPlaceRecoveryAnimData;
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Setup_TurnInPlaceRecovery(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void Update_TurnInPlaceRecovery(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);


	UPROPERTY(EditDefaultsOnly, Category="Cancer")
	FVector2D PlayRateDefault{0.75f, 1.25f};

	UPROPERTY(EditDefaultsOnly, Category="Cancer|Anim Chooser Table")
	TObjectPtr<UChooserTable> IdleAnimChooserTable;

	UPROPERTY(EditDefaultsOnly, Category="Cancer|Anim Chooser Table")
	TObjectPtr<UChooserTable> StartAnimChooserTable;

	UPROPERTY(EditDefaultsOnly, Category="Cancer|Anim Chooser Table")
	TObjectPtr<UChooserTable> CycleAnimChooserTable;

	UPROPERTY(EditDefaultsOnly, Category="Cancer|Anim Chooser Table")
	TObjectPtr<UChooserTable> StopAnimChooserTable;

	UPROPERTY(EditDefaultsOnly, Category="Cancer|Anim Chooser Table")
	TObjectPtr<UChooserTable> PivotAnimChooserTable;

	UPROPERTY(EditDefaultsOnly, Category="Cancer|Anim Chooser Table")
	TObjectPtr<UChooserTable> CrossTurnAnimChooserTable;

	UPROPERTY(EditDefaultsOnly, Category="Cancer|Anim Chooser Table")
	TObjectPtr<UChooserTable> TurnPlaceAnimChooserTable;

	UPROPERTY(EditDefaultsOnly, Category="Cancer|Anim Chooser Table")
	TObjectPtr<UChooserTable> JumpStartAnimChooserTable;

	UPROPERTY(EditDefaultsOnly, Category="Cancer|Anim Chooser Table")
	TObjectPtr<UChooserTable> DoubleJumpAnimChooserTable;

	UPROPERTY(EditDefaultsOnly, Category="Cancer|Anim Chooser Table")
	TObjectPtr<UChooserTable> JumpLoopAnimChooserTable;

	UPROPERTY(EditDefaultsOnly, Category="Cancer|Anim Chooser Table")
	TObjectPtr<UChooserTable> FallLandAnimChooserTable;


#pragma region 辅助函数

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	UAnimSequenceBase* GetAnimSequence(const UObject* ContextObject, UChooserTable* AnimChooserTable);

	void SetUpAnim(FCurrentAnimData& AnimData);

	void UpdateCycleByDir(const FAnimUpdateContext& Context, const FAnimNodeReference& Node,
	                      ECancerDirection_6 Direction);
	ECancerDirection_8 CalculateVelocityDirection(float Angle) const;
	UFUNCTION(BlueprintPure, Category = "Cancer|Linked Animation Instance",
		Meta = (BlueprintThreadSafe, ReturnDisplayName = "CancerAnimInstance"))
	UCancerAnimInstance* GetParent() const;
	UE_DEPRECATED(4.14, "Please use GetParent() instead")
	UFUNCTION(BlueprintPure, Category = "Cancer|Linked Animation Instance",
		Meta = (DeprecatedFunction, DeprecationMessage = "Please use GetParent() instead."))
	UCancerAnimInstance* GetParentUnsafe() const;
	UFUNCTION(BlueprintPure, Category = "Cancer|Linked CharacterMovement",
		Meta = (BlueprintThreadSafe, ReturnDisplayName = "CancerMovement"))
	UCancerMovementComponent* GetParentMovement() const;


#pragma endregion
};
