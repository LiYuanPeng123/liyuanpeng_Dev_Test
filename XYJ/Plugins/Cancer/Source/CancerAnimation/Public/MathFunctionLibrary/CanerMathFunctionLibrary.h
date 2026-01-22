
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CanerMathFunctionLibrary.generated.h"

/**
 * 结构体用于存储弹簧阻尼动画的状态信息
 * 包含速度、前一个目标值和状态有效性标志
 */
USTRUCT(BlueprintType)
struct CANCERANIMATION_API FCancerSpringFloatState
{
	GENERATED_BODY()

public:
	/** 当前速度，用于计算动画的运动速度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer")
	float Velocity{ForceInit};

	/** 上一帧的目标值，用于计算目标速度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer")
	float PreviousTarget{ForceInit};

	/** 状态有效性标志，用于判断是否需要重置状态 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer")
	uint8 bStateValid : 1 {false};

public:
	/** 重置弹簧状态，将所有值恢复到初始状态 */
	void Reset();
};



/**
 * 动画数学工具类，提供各种动画相关的数学计算功能
 * 包含插值、阻尼、弹簧动画等常用动画计算方法
 */
UCLASS(Meta = (BlueprintThreadSafe))
class CANCERANIMATION_API UCanerMathFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    /**
     * 将数值限制在0到1之间
     * @param Value 需要限制的数值
     * @return 限制后的数值（0到1之间）
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Math Utility", Meta = (ReturnDisplayName = "Value"))
    static float Clamp01(float Value);

    /**
     * 在两个值之间进行线性插值，并限制结果在0到1之间
     * @param From 起始值
     * @param To 目标值
     * @param Ratio 插值比例（0到1）
     * @return 插值结果
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Math Utility", Meta = (ReturnDisplayName = "Value"))
    static float LerpClamped(float From, float To, float Ratio);

    /**
     * 模板函数，用于实现值的阻尼效果
     * @param Current 当前值
     * @param Target 目标值
     * @param DeltaTime 帧时间
     * @param Smoothing 平滑度参数
     * @return 平滑后的值
     */
    template <typename ValueType>
    static ValueType Damp(const ValueType& Current, const ValueType& Target, float DeltaTime, float Smoothing);

    /**
     * 计算阻尼效果的插值比例
     * @param DeltaTime 帧时间
     * @param Smoothing 平滑度参数
     * @return 插值比例
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Math Utility", Meta = (ReturnDisplayName = "Interpolation Amount"))
    static float Damp(float DeltaTime, float Smoothing);

    /**
     * 模板函数，实现值的指数衰减效果
     * @param Current 当前值
     * @param Target 目标值
     * @param DeltaTime 帧时间
     * @param Lambda 衰减系数
     * @return 衰减后的值
     */
    template <typename ValueType>
    static ValueType ExponentialDecay(const ValueType& Current, const ValueType& Target, float DeltaTime, float Lambda);

    /**
     * 计算指数衰减的插值比例
     * @param DeltaTime 帧时间
     * @param Lambda 衰减系数
     * @return 衰减比例
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Math Utility", Meta = (ReturnDisplayName = "Interpolation Amount"))
    static float ExponentialDecay(float DeltaTime, float Lambda);

    /**
     * 模板函数，实现弹簧阻尼动画效果
     * @param SpringState 弹簧状态
     * @param Current 当前值
     * @param Target 目标值
     * @param DeltaTime 帧时间
     * @param Frequency 频率
     * @param DampingRatio 阻尼比
     * @param TargetVelocityAmount 目标速度比例
     * @return 计算后的值
     */
    template <typename ValueType, typename StateType>
    static ValueType SpringDamp(StateType& SpringState, const ValueType& Current, const ValueType& Target,
                               float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount = 1.0f);

    /**
     * 计算浮点数的弹簧阻尼效果
     * @param SpringState 弹簧状态
     * @param Current 当前值
     * @param Target 目标值
     * @param DeltaTime 帧时间
     * @param Frequency 频率
     * @param DampingRatio 阻尼比
     * @param TargetVelocityAmount 目标速度比例
     * @return 计算后的值
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Math Utility", Meta = (ReturnDisplayName = "Value"))
    static float SpringDampFloat(UPARAM(ref) FCancerSpringFloatState& SpringState, float Current, float Target,
                                 float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount = 1.0f);

    /**
     * 计算运动方向
     * @param Angle 当前角度
     * @param ForwardHalfAngle 前向半角
     * @param AngleThreshold 角度阈值
     * @return 运动方向
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Math Utility", Meta = (ReturnDisplayName = "Direction"))
    static ECancerDirection_4 CalculateMovementDirection(float Angle, float ForwardHalfAngle, float AngleThreshold);

    /**
     * 计算极向量的位置和方向
     * @param ALocation A点位置
     * @param BLocation B点位置
     * @param CLocation C点位置
     * @param ProjectionLocation 投影位置
     * @param PoleDirection 极向量方向
     * @return 是否计算成功
     */
    UFUNCTION(BlueprintCallable, Category = "Cancer|Math Utility",
        Meta = (AutoCreateRefTerm = "ALocation, BLocation, CLocation", ExpandBoolAsExecs = "ReturnValue"))
    static bool TryCalculatePoleVector(const FVector& ALocation, const FVector& BLocation, const FVector& CLocation,
                                       FVector& ProjectionLocation, FVector& PoleDirection);
};

inline void FCancerSpringFloatState::Reset()
{
    Velocity = 0.f;
    PreviousTarget = 0.f;
    bStateValid = false;
}

inline float UCanerMathFunctionLibrary::Clamp01(const float Value)
{
    return Value <= 0.0f
               ? 0.0f
               : Value >= 1.0f
               ? 1.0f
               : Value;
}

inline float UCanerMathFunctionLibrary::LerpClamped(const float From, const float To, const float Ratio)
{
    return From + (To - From) * Clamp01(Ratio);
}

template <typename ValueType>
ValueType UCanerMathFunctionLibrary::Damp(const ValueType& Current, const ValueType& Target, const float DeltaTime, const float Smoothing)
{
    return Smoothing > 0.0f
               ? FMath::Lerp(Current, Target, Damp(DeltaTime, Smoothing))
               : Target;
}

inline float UCanerMathFunctionLibrary::Damp(const float DeltaTime, const float Smoothing)
{
    // https://www.rorydriscoll.com/2016/03/07/frame-rate-independent-damping-using-lerp/

    return 1.0f - FMath::Pow(Smoothing, DeltaTime);
}

inline float UCanerMathFunctionLibrary::ExponentialDecay(const float DeltaTime, const float Lambda)
{
    // https://www.rorydriscoll.com/2016/03/07/frame-rate-independent-damping-using-lerp/

    return 1.0f - FMath::InvExpApprox(Lambda * DeltaTime);
}


template <typename ValueType>
ValueType UCanerMathFunctionLibrary::ExponentialDecay(const ValueType& Current, const ValueType& Target, const float DeltaTime, const float Lambda)
{
    return Lambda > 0.0f
               ? FMath::Lerp(Current, Target, ExponentialDecay(DeltaTime, Lambda))
               : Target;
}

template <typename ValueType, typename StateType>
ValueType UCanerMathFunctionLibrary::SpringDamp(StateType& SpringState, const ValueType& Current, const ValueType& Target, const float DeltaTime,
                               const float Frequency, const float DampingRatio, const float TargetVelocityAmount)
{
    if (DeltaTime <= UE_SMALL_NUMBER)
    {
        return Current;
    }

    if (!SpringState.bStateValid)
    {
        SpringState.Velocity = ValueType{0.0f};
        SpringState.PreviousTarget = Target;
        SpringState.bStateValid = true;

        return Target;
    }

    ValueType Result{Current};
    FMath::SpringDamper(Result, SpringState.Velocity, Target,
                        (Target - SpringState.PreviousTarget) * Clamp01(TargetVelocityAmount) / DeltaTime,
                        DeltaTime, Frequency, DampingRatio);

    SpringState.PreviousTarget = Target;

    return Result;
}