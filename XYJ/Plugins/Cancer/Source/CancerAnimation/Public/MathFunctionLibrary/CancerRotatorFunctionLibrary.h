// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CanerMathFunctionLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerRotatorFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CANCERANIMATION_API UCancerRotatorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    /**
     * 逆时针旋转的角度阈值（5度）
     * 用于判断是否需要进行逆时针旋转
     */
    static constexpr auto CounterClockwiseRotationAngleThreshold{5.0f};

public:
    /**
     * 模板函数，用于将角度重映射为逆时针旋转
     * @param Angle 需要重映射的角度值
     * @return 重映射后的角度值
     * 
     * 该函数将角度从[175, 180]范围重映射到[-185, -180]范围，
     * 用于在角色进行180度转身时实现逆时针旋转效果
     */
    template <typename ValueType> requires std::is_floating_point_v<ValueType>
    static constexpr ValueType RemapAngleForCounterClockwiseRotation(ValueType Angle);

    /**
     * 将角度重映射为逆时针旋转
     * @param Angle 需要重映射的角度值
     * @return 重映射后的角度值
     * 
     * 该函数将角度从[175, 180]范围重映射到[-185, -180]范围，
     * 用于在角色进行180度转身时实现逆时针旋转效果
     */
    UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
    static float RemapAngleForCounterClockwiseRotation(float Angle);

    /**
     * 在两个角度之间进行插值
     * @param From 起始角度
     * @param To 目标角度
     * @param Ratio 插值比例（0到1）
     * @return 插值后的角度值
     * 
     * 该函数会自动处理角度的循环特性（如350度到10度的插值）
     */
    UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
    static float LerpAngle(float From, float To, float Ratio);

    /**
     * 在两个旋转之间进行插值
     * @param From 起始旋转
     * @param To 目标旋转
     * @param Ratio 插值比例（0到1）
     * @return 插值后的旋转
     * 
     * 该函数会自动处理旋转的循环特性
     */
    UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (AutoCreateRefTerm = "From, To", ReturnDisplayName = "Rotation"))
    static FRotator LerpRotation(const FRotator& From, const FRotator& To, float Ratio);

    /**
     * 以恒定速度插值角度
     * @param Current 当前角度
     * @param Target 目标角度
     * @param DeltaTime 帧时间
     * @param Speed 旋转速度
     * @return 插值后的角度值
     * 
     * 该函数以恒定速度向目标角度插值，适用于需要平滑旋转的场景
     */
    UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
    static float InterpolateAngleConstant(float Current, float Target, float DeltaTime, float Speed);

    /**
     * 计算角度的阻尼效果
     * @param Current 当前角度
     * @param Target 目标角度
     * @param DeltaTime 帧时间
     * @param Smoothing 平滑度参数
     * @return 阻尼后的角度值
     * 
     * 该函数实现角度的阻尼效果，使旋转更加自然
     */
    UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
    static float DampAngle(float Current, float Target, float DeltaTime, float Smoothing);

    /**
     * 计算旋转的阻尼效果
     * @param Current 当前旋转
     * @param Target 目标旋转
     * @param DeltaTime 帧时间
     * @param Smoothing 平滑度参数
     * @return 阻尼后的旋转
     * 
     * 该函数实现旋转的阻尼效果，使旋转更加自然
     */
    UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility",
        Meta = (AutoCreateRefTerm = "Current, Target", ReturnDisplayName = "Rotation"))
    static FRotator DampRotation(const FRotator& Current, const FRotator& Target, float DeltaTime, float Smoothing);

    /**
     * 计算角度的指数衰减效果
     * @param Current 当前角度
     * @param Target 目标角度
     * @param DeltaTime 帧时间
     * @param Lambda 衰减系数
     * @return 衰减后的角度值
     * 
     * 该函数实现角度的指数衰减效果，使旋转更加自然
     */
    UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
    static float ExponentialDecayAngle(float Current, float Target, float DeltaTime, float Lambda);

    /**
     * 计算旋转的指数衰减效果
     * @param Current 当前旋转
     * @param Target 目标旋转
     * @param DeltaTime 帧时间
     * @param Lambda 衰减系数
     * @return 衰减后的旋转
     * 
     * 该函数实现旋转的指数衰减效果，使旋转更加自然
     */
    UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility",
        Meta = (AutoCreateRefTerm = "Current, Target", ReturnDisplayName = "Rotation"))
    static FRotator ExponentialDecayRotation(const FRotator& Current, const FRotator& Target, float DeltaTime, float Lambda);

    /**
     * 使用快速线性插值的四元数插值
     * 与FMath::QInterpTo()类似，但使用FQuat::FastLerp()替代FQuat::Slerp()
     * 该方法计算速度更快，但可能在某些情况下精度略低
     */
    UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Quaternion"))
    static FQuat InterpolateQuaternionFast(const FQuat& Current, const FQuat& Target, float DeltaTime, float Speed);

    /**
     * 获取四元数的扭转部分
     * @param Quaternion 四元数
     * @param TwistAxis 扭转轴
     * @return 扭转部分的四元数
     * 
     * 该函数用于分离四元数的旋转和扭转部分
     */
    UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (AutoCreateRefTerm = "TwistAxis", ReturnDisplayName = "Twist"))
    static FQuat GetTwist(const FQuat& Quaternion, const FVector& TwistAxis = FVector::UpVector);
};

template <typename ValueType> requires std::is_floating_point_v<ValueType>
constexpr ValueType UCancerRotatorFunctionLibrary::RemapAngleForCounterClockwiseRotation(const ValueType Angle)
{
    if (Angle > 180.0f - CounterClockwiseRotationAngleThreshold)
    {
        return Angle - 360.0f;
    }

    return Angle;
}

inline float UCancerRotatorFunctionLibrary::RemapAngleForCounterClockwiseRotation(const float Angle)
{
    return RemapAngleForCounterClockwiseRotation<float>(Angle);
}

inline float UCancerRotatorFunctionLibrary::LerpAngle(const float From, const float To, const float Ratio)
{
    auto Delta{FMath::UnwindDegrees(To - From)};
    Delta = RemapAngleForCounterClockwiseRotation(Delta);

    return FMath::UnwindDegrees(From + Delta * Ratio);
}

inline FRotator UCancerRotatorFunctionLibrary::LerpRotation(const FRotator& From, const FRotator& To, const float Ratio)
{
    auto Result{To - From};
    Result.Normalize();

    Result.Pitch = RemapAngleForCounterClockwiseRotation(Result.Pitch);
    Result.Yaw = RemapAngleForCounterClockwiseRotation(Result.Yaw);
    Result.Roll = RemapAngleForCounterClockwiseRotation(Result.Roll);

    Result *= Ratio;
    Result += From;
    Result.Normalize();

    return Result;
}

inline float UCancerRotatorFunctionLibrary::InterpolateAngleConstant(const float Current, const float Target, const float DeltaTime, const float Speed)
{
    if (Speed <= 0.0f || FMath::IsNearlyEqual(Current, Target))
    {
        return Target;
    }

    auto Delta{FMath::UnwindDegrees(Target - Current)};
    Delta = RemapAngleForCounterClockwiseRotation(Delta);

    const auto MaxDelta{Speed * DeltaTime};

    return FMath::UnwindDegrees(Current + FMath::Clamp(Delta, -MaxDelta, MaxDelta));
}

inline float UCancerRotatorFunctionLibrary::DampAngle(const float Current, const float Target, const float DeltaTime, const float Smoothing)
{
    return Smoothing > 0.0f
               ? LerpAngle(Current, Target, UCanerMathFunctionLibrary::Damp(DeltaTime, Smoothing))
               : Target;
}

inline float UCancerRotatorFunctionLibrary::ExponentialDecayAngle(const float Current, const float Target, const float DeltaTime, const float Lambda)
{
    return Lambda > 0.0f
               ? LerpAngle(Current, Target, UCanerMathFunctionLibrary::ExponentialDecay(DeltaTime, Lambda))
               : Target;
}

inline FRotator UCancerRotatorFunctionLibrary::DampRotation(const FRotator& Current, const FRotator& Target, const float DeltaTime, const float Smoothing)
{
    return Smoothing > 0.0f
               ? LerpRotation(Current, Target, UCanerMathFunctionLibrary::Damp(DeltaTime, Smoothing))
               : Target;
}

inline FRotator UCancerRotatorFunctionLibrary::ExponentialDecayRotation(const FRotator& Current, const FRotator& Target,
                                                       const float DeltaTime, const float Lambda)
{
    return Lambda > 0.0f
               ? LerpRotation(Current, Target, UCanerMathFunctionLibrary::ExponentialDecay(DeltaTime, Lambda))
               : Target;
}

inline FQuat UCancerRotatorFunctionLibrary::InterpolateQuaternionFast(const FQuat& Current, const FQuat& Target, const float DeltaTime, const float Speed)
{
    if (Speed <= 0.0f || Current.Equals(Target))
    {
        return Target;
    }

    return FQuat::FastLerp(Current, Target, UCanerMathFunctionLibrary::Clamp01(Speed * DeltaTime)).GetNormalized();
}

inline FQuat UCancerRotatorFunctionLibrary::GetTwist(const FQuat& Quaternion, const FVector& TwistAxis)
{
    // Based on TQuat<T>::ToSwingTwist().

    const auto Projection{(TwistAxis | FVector{Quaternion.X, Quaternion.Y, Quaternion.Z}) * TwistAxis};

    return FQuat{Projection.X, Projection.Y, Projection.Z, Quaternion.W}.GetNormalized();
}
