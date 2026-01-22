
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerVectorFunctionLibrary.generated.h"

/**
 * 结构体用于存储向量弹簧阻尼动画的状态信息
 * 包含速度、前一个目标值和状态有效性标志
 */
USTRUCT(BlueprintType)
struct CANCERANIMATION_API FCancerSpringVectorState
{
	GENERATED_BODY()

public:
	/** 当前速度向量，用于计算动画的运动速度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer")
	FVector Velocity{ForceInit};

	/** 上一帧的目标向量，用于计算目标速度 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer")
	FVector PreviousTarget{ForceInit};

	/** 状态有效性标志，用于判断是否需要重置状态 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer")
	uint8 bStateValid : 1 {false};

public:
	/** 重置向量弹簧状态，将所有值恢复到初始状态 */
	void Reset();
};


UCLASS(Meta = (BlueprintThreadSafe))
class CANCERANIMATION_API UCancerVectorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    /**
     * 将向量的大小限制在0到1之间
     * @param Vector 需要限制的向量
     * @return 限制大小后的向量
     * 
     * 该函数保持向量的方向不变，只限制其大小
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", Meta = (AutoCreateRefTerm = "Vector", ReturnDisplayName = "Vector"))
    static FVector ClampMagnitude01(const FVector& Vector);

    /**
     * 将单精度浮点向量的大小限制在0到1之间
     * @param Vector 需要限制的向量
     * @return 限制大小后的向量
     * 
     * 该函数保持向量的方向不变，只限制其大小
     */
    static FVector3f ClampMagnitude01(const FVector3f& Vector);

    /**
     * 将2D向量的大小限制在0到1之间
     * @param Vector 需要限制的向量
     * @return 限制大小后的向量
     * 
     * 该函数保持向量的方向不变，只限制其大小
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", DisplayName = "Clamp Magnitude 01 2D",
        Meta = (AutoCreateRefTerm = "Vector", ReturnDisplayName = "Vector"))
    static FVector2D ClampMagnitude012D(const FVector2D& Vector);

    /**
     * 将弧度转换为2D方向向量
     * @param Radian 弧度值
     * @return 对应的2D方向向量
     * 
     * 该函数将弧度值转换为单位长度的2D方向向量
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", Meta = (ReturnDisplayName = "Direction"))
    static FVector2D RadianToDirection(float Radian);

    /**
     * 将弧度转换为3D方向向量（仅XY平面）
     * @param Radian 弧度值
     * @return 对应的3D方向向量
     * 
     * 该函数将弧度值转换为单位长度的3D方向向量，Z轴始终为0
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", Meta = (ReturnDisplayName = "Direction"))
    static FVector RadianToDirectionXY(float Radian);

    /**
     * 将角度转换为2D方向向量
     * @param Angle 角度值（以度为单位）
     * @return 对应的2D方向向量
     * 
     * 该函数将角度值转换为单位长度的2D方向向量
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", Meta = (ReturnDisplayName = "Direction"))
    static FVector2D AngleToDirection(float Angle);

    /**
     * 将角度转换为3D方向向量（仅XY平面）
     * @param Angle 角度值（以度为单位）
     * @return 对应的3D方向向量
     * 
     * 该函数将角度值转换为单位长度的3D方向向量，Z轴始终为0
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", Meta = (ReturnDisplayName = "Direction"))
    static FVector AngleToDirectionXY(float Angle);

    /**
     * 计算2D向量与X轴的夹角
     * @param Direction 2D向量
     * @return 夹角值（以度为单位）
     * 
     * 该函数计算2D向量与X轴的夹角，范围为[-180, 180]
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", Meta = (AutoCreateRefTerm = "Direction", ReturnDisplayName = "Angle"))
    static double DirectionToAngle(const FVector2D& Direction);

    /**
     * 计算3D向量与X轴的夹角（仅XY平面）
     * @param Direction 3D向量
     * @return 夹角值（以度为单位）
     * 
     * 该函数计算3D向量与X轴的夹角，范围为[-180, 180]
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", Meta = (AutoCreateRefTerm = "Direction", ReturnDisplayName = "Angle"))
    static double DirectionToAngleXY(const FVector& Direction);

    /**
     * 计算向量的顺时针垂直向量（仅XY平面）
     * @param Vector 向量
     * @return 顺时针垂直向量
     * 
     * 该函数计算向量的顺时针垂直向量，Z轴始终为0
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", Meta = (AutoCreateRefTerm = "Vector", ReturnDisplayName = "Vector"))
    static FVector PerpendicularClockwiseXY(const FVector& Vector);

    /**
     * 计算向量的逆时针垂直向量（仅XY平面）
     * @param Vector 向量
     * @return 逆时针垂直向量
     * 
     * 该函数计算向量的逆时针垂直向量，Z轴始终为0
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", Meta = (AutoCreateRefTerm = "Vector", ReturnDisplayName = "Vector"))
    static FVector PerpendicularCounterClockwiseXY(const FVector& Vector);

    /**
     * 计算两个向量之间的夹角（不进行归一化）
     * @param From 向量1
     * @param To 向量2
     * @return 夹角值（以度为单位）
     * 
     * 该函数计算两个向量之间的夹角，不进行归一化
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", DisplayName = "Angle Between (Skip Normalization)",
        Meta = (AutoCreateRefTerm = "From, To", ReturnDisplayName = "Angle"))
    static double AngleBetweenSkipNormalization(const FVector& From, const FVector& To);

    /**
     * 计算两个向量之间的有符号夹角（仅XY平面）
     * @param From 向量1
     * @param To 向量2
     * @return 夹角值（以度为单位）
     * 
     * 该函数计算两个向量之间的有符号夹角，仅XY平面
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", Meta = (AutoCreateRefTerm = "From, To", ReturnDisplayName = "Angle"))
    static float AngleBetweenSignedXY(const FVector3f& From, const FVector3f& To);

    /**
     * 计算两个向量之间的球面插值（不进行归一化）
     * @param From 向量1
     * @param To 向量2
     * @param Ratio 插值系数
     * @return 插值后的向量
     * 
     * 该函数计算两个向量之间的球面插值，不进行归一化
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", DisplayName = "Slerp (Skip Normalization)",
        Meta = (AutoCreateRefTerm = "From, To", ReturnDisplayName = "Direction"))
    static FVector SlerpSkipNormalization(const FVector& From, const FVector& To, float Ratio);

    /**
     * 计算向量的弹簧阻尼效果
     * @param SpringState 弹簧状态
     * @param Current 当前向量
     * @param Target 目标向量
     * @param DeltaTime 帧时间
     * @param Frequency 频率
     * @param DampingRatio 阻尼比
     * @param TargetVelocityAmount 目标速度比例
     * @return 计算后的向量
     * 
     * 该函数实现向量的弹簧阻尼效果，使运动具有弹性
     */
    UFUNCTION(BlueprintPure, Category = "Cancer|Vector Utility", Meta = (AutoCreateRefTerm = "SpringState", ReturnDisplayName = "Vector"))
    static FVector SpringDampVector(UPARAM(ref) FCancerSpringVectorState& SpringState, const FVector& Current, const FVector& Target,
                                   float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount = 1.0f);
};

inline void FCancerSpringVectorState::Reset()
{
    Velocity = FVector::ZeroVector;
    PreviousTarget = FVector::ZeroVector;
    bStateValid = false;
}

inline FVector UCancerVectorFunctionLibrary::ClampMagnitude01(const FVector& Vector)
{
    const auto MagnitudeSquared{Vector.SizeSquared()};

    if (MagnitudeSquared <= 1.0f)
    {
        return Vector;
    }

    const auto Scale{FMath::InvSqrt(MagnitudeSquared)};

    return {Vector.X * Scale, Vector.Y * Scale, Vector.Z * Scale};
}

inline FVector3f UCancerVectorFunctionLibrary::ClampMagnitude01(const FVector3f& Vector)
{
    const auto MagnitudeSquared{Vector.SizeSquared()};

    if (MagnitudeSquared <= 1.0f)
    {
        return Vector;
    }

    const auto Scale{FMath::InvSqrt(MagnitudeSquared)};

    return {Vector.X * Scale, Vector.Y * Scale, Vector.Z * Scale};
}

inline FVector2D UCancerVectorFunctionLibrary::ClampMagnitude012D(const FVector2D& Vector)
{
    const auto MagnitudeSquared{Vector.SizeSquared()};

    if (MagnitudeSquared <= 1.0f)
    {
        return Vector;
    }

    const auto Scale{FMath::InvSqrt(MagnitudeSquared)};

    return {Vector.X * Scale, Vector.Y * Scale};
}

inline FVector2D UCancerVectorFunctionLibrary::RadianToDirection(const float Radian)
{
    float Sin, Cos;
    FMath::SinCos(&Sin, &Cos, Radian);

    return {Cos, Sin};
}

inline FVector UCancerVectorFunctionLibrary::RadianToDirectionXY(const float Radian)
{
    float Sin, Cos;
    FMath::SinCos(&Sin, &Cos, Radian);

    return {Cos, Sin, 0.0f};
}

inline FVector2D UCancerVectorFunctionLibrary::AngleToDirection(const float Angle)
{
    return RadianToDirection(FMath::DegreesToRadians(Angle));
}

inline FVector UCancerVectorFunctionLibrary::AngleToDirectionXY(const float Angle)
{
    return RadianToDirectionXY(FMath::DegreesToRadians(Angle));
}

inline double UCancerVectorFunctionLibrary::DirectionToAngle(const FVector2D& Direction)
{
    return FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
}

inline double UCancerVectorFunctionLibrary::DirectionToAngleXY(const FVector& Direction)
{
    return FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
}

inline FVector UCancerVectorFunctionLibrary::PerpendicularClockwiseXY(const FVector& Vector)
{
    return {Vector.Y, -Vector.X, Vector.Z};
}

inline FVector UCancerVectorFunctionLibrary::PerpendicularCounterClockwiseXY(const FVector& Vector)
{
    return {-Vector.Y, Vector.X, Vector.Z};
}

inline double UCancerVectorFunctionLibrary::AngleBetweenSkipNormalization(const FVector& From, const FVector& To)
{
    return FMath::RadiansToDegrees(FMath::Acos(From | To));
}

inline float UCancerVectorFunctionLibrary::AngleBetweenSignedXY(const FVector3f& From, const FVector3f& To)
{
    const auto FromXY{FVector2f{From}.GetSafeNormal()};
    const auto ToXY{FVector2f{To}.GetSafeNormal()};

    // return FMath::RadiansToDegrees(FMath::Atan2(FromXY ^ ToXY, FromXY | ToXY));

    return FMath::RadiansToDegrees(FMath::Acos(FromXY | ToXY)) * FMath::Sign(FromXY ^ ToXY);
}


