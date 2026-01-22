
#pragma once

#include "CoreMinimal.h"
#include "SequenceEvaluatorLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/AnimInstanceDefinition.h"
#include "CancerAnimationFunctionLibrary.generated.h"

class UCancerLocomotionComponent;
class UCancerMovementComponent;
class UCancerAnimInstance;



/**
 *  Cancer AnimationFunctionLibrary
 */
UCLASS()
class CANCERANIMATION_API UCancerAnimationFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	public:
	/**
	 *蓝图线程安全打印
    */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "CancerAnimationFunctionLibrary")
	static void PrintStrOnThreadSafe(const FString& Str,const FColor& Color = FColor::Red);

	/**
     * 计算速度混合值
     * @param Actor 演员对象
     * @return 速度混合值
     * 
     * 该函数用于计算演员的当前速度混合值，用于动画混合
     */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe), Category = "CancerAnimationFunctionLibrary")
    static FCancerVelocityBlend CalculateVelocityBlend(const AActor* Actor);

    /**
     * 速度混合值插值
     * @param Current 当前混合值
     * @param Target 目标混合值
     * @param InterpSpeed 插值速度
     * @param DeltaTime 帧时间
     * @return 插值后的混合值
     * 
     * 该函数用于在两个速度混合值之间进行平滑插值
     */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe), Category = "CancerAnimationFunctionLibrary")
    static FCancerVelocityBlend InterpVelocityBlend(FCancerVelocityBlend Current, FCancerVelocityBlend Target, float InterpSpeed, float DeltaTime);

    /**
     * 计算速度方向角度
     * @param Dir 方向向量
     * @param Actor 演员对象
     * @return 速度方向角度
     * 
     * 该函数用于计算演员的当前速度方向角度
     */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe), Category = "CancerAnimationFunctionLibrary")
    static float CalculateDirectionVelocityAngle(const FVector& Dir, const AActor* Actor);

    /**
     * 计算加速度方向角度
     * @param Dir 方向向量
     * @param Actor 演员对象
     * @return 加速度方向角度
     * 
     * 该函数用于计算演员的当前加速度方向角度
     */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe), Category = "CancerAnimationFunctionLibrary")
    static float CalculateDirectionAccelerationAngle(const FVector& Dir, const AActor* Actor);

    /**
     * 计算象限方向
     * @param Current 当前方向
     * @param FR 右前方阈值
     * @param FL 左前方阈值
     * @param BR 右后方阈值
     * @param BL 左后方阈值
     * @param Buffer 缓冲值
     * @param Angle 当前角度
     * @return 计算后的方向
     * 
     * 该函数用于根据角度和阈值计算当前所在象限
     */
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe), Category = "CancerAnimationFunctionLibrary")
    static ECancerDirection_4 CalculateQuadrant(ECancerDirection_4 Current,
                                            float FR, float FL, float BR, float BL, float Buffer, float Angle);

	//通过角度计算8方向
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe), Category = "CancerAnimationFunctionLibrary")
	static ECancerDirection_8 CalculateDirByAngle(float Angle);

    /**
     * 判断角度是否在范围内
     * @param Angle 当前角度
     * @param Min 最小角度
     * @param Max 最大角度
     * @param Buffer 缓冲值
     * @param IncreaseBuffer 是否增加缓冲
     * @return 是否在范围内
     * 
     * 该函数用于判断一个角度是否在指定范围内，考虑缓冲值
     */
    static bool AngleInRange(float Angle, float Min, float Max, float Buffer, bool IncreaseBuffer);

    /**
     * 从角色获取动画曲线值
     * @param Character 角色对象
     * @param CurveName 曲线名称
     * @return 曲线值
     * 
     * 该函数用于从角色的动画中获取指定曲线的值
     */
    UFUNCTION(BlueprintPure, Category = "CancerAnimationFunctionLibrary",
        Meta = (DefaultToSelf = "Character", AutoCreateRefTerm = "CurveName", ReturnDisplayName = "Curve Value"))
    static float GetAnimationCurveValueFromCharacter(const ACharacter* Character, const FName& CurveName);


	UFUNCTION(BlueprintPure,DisplayName="预测停止距离",Category = "CancerAnimationFunctionLibrary", meta = (BlueprintThreadSafe))
	static FVector PredictGroundStopLocation(const UCharacterMovementComponent*MovementComponent);

	UFUNCTION(BlueprintPure,DisplayName="预测角色转向位置", Category = "CancerAnimationFunctionLibrary",meta = (BlueprintThreadSafe))
	static FVector PredictGroundPivotLocation(const UCharacterMovementComponent*MovementComponent);

	//播放时间和旋转角度匹配
	UFUNCTION(BlueprintCallable, Category = "RemainingTurnYaw Matching", meta=(BlueprintThreadSafe))
	static FSequenceEvaluatorReference AdvanceTimeByRemainingTurnYaw(const FAnimUpdateContext& UpdateContext, const FSequenceEvaluatorReference& SequenceEvaluator,
		float InYaw, FName RemainingTurnYawCurveName, FVector2D PlayRateClamp = FVector2D(0.75f, 1.25f));

	 static float GetAnimPositionFromYaw(const UAnimSequenceBase*InAnimSequence,const float& InYaw,
	 	FName InCurveName);
	
#pragma  region 获取 CancerAnimInstance
	template<class T = UCancerAnimInstance>
	static T*GetCancerAnimInstance(AActor* Actor)
	{
		const ACharacter* Character = Cast<ACharacter>(Actor);
		if (!IsValid(Character)&&!Character->GetMesh()) return nullptr;
		return Cast<T>(Character->GetMesh()->GetAnimInstance());
	}

	UFUNCTION(BlueprintCallable, Category = "Cancer", DisplayName="Get Cancer Anim Instance",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static UCancerAnimInstance* K2_GetCancerAnimInstance(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerAnimInstance> OutputClass = nullptr);
#pragma endregion

#pragma  region 获取 CancerMovementComponent
	template<class T = UCancerMovementComponent>
	static T*GetCancerMovementComponent(AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;
		return Actor->GetComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Cancer", DisplayName="Get Cancer Movement Component",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static UCancerMovementComponent* K2_GetCancerMovementComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerMovementComponent> OutputClass = nullptr);
#pragma endregion

#pragma region  获取CancerLocomotionComponent
	template <class T = UCancerLocomotionComponent>
	static T* GetCancerLocomotionComponent(AActor*Actor)
	{
		if (!IsValid(Actor)) return nullptr;
		return Actor->GetComponentByClass<T>();
	}
	
	UFUNCTION(BlueprintCallable, Category = "Cancer", DisplayName="Get Cancer Locomotion Component",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static UCancerLocomotionComponent* K2_CancerLocomotionComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerLocomotionComponent> OutputClass = nullptr);
	
#pragma endregion
};
