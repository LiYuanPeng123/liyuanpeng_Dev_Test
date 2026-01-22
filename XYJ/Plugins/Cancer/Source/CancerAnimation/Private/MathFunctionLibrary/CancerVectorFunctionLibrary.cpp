
#include "MathFunctionLibrary/CancerVectorFunctionLibrary.h"

#include "MathFunctionLibrary/CanerMathFunctionLibrary.h"


/**
 * 球面线性插值（Slerp）
 * 
 * 该函数实现向量的球面线性插值，用于动画的平滑过渡
 * 主要步骤：
 * 1. 计算向量的点积
 * 2. 判断是否近似共线
 * 3. 计算插值角度
 * 4. 使用三角函数计算插值结果
 * 
 * @param From 起始向量
 * @param To 目标向量
 * @param Ratio 插值比例（0-1）
 * @return 插值结果
 * 
 * 参考：https://allenchou.net/2018/05/game-math-deriving-the-slerp-formula/
 */
 FVector UCancerVectorFunctionLibrary::SlerpSkipNormalization(const FVector& From, const FVector& To, float Ratio)
{
 	// 计算向量的点积
 	const auto Dot{From | To};

 	// 判断是否近似共线（点积接近1或-1）
 	if (Dot > 0.9995f || Dot < -0.9995f)
 	{
 		// 近似共线时使用线性插值
 		return FMath::Lerp(From, To, Ratio).GetSafeNormal();
 	}

 	// 计算插值角度
 	const auto Theta{UE_REAL_TO_FLOAT(FMath::Acos(Dot)) * Ratio};

 	// 计算三角函数值
 	float Sin, Cos;
 	FMath::SinCos(&Sin, &Cos, Theta);

 	// 计算从向量到目标向量的垂直分量
 	const auto FromPerpendicular{(To - From * Dot).GetSafeNormal()};

 	// 使用三角函数计算插值结果
 	return From * Cos + FromPerpendicular * Sin;
}

/**
 * 向量的弹簧阻尼效果
 * 
 * 该函数使用模板函数实现向量的弹簧阻尼效果
 * 主要用于动画的平滑过渡
 * 
 * @param SpringState 弹簧状态
 * @param Current 当前值
 * @param Target 目标值
 * @param DeltaTime 帧时间
 * @param Frequency 频率
 * @param DampingRatio 阻尼比
 * @param TargetVelocityAmount 目标速度比例
 * @return 计算后的值
 */
 FVector UCancerVectorFunctionLibrary::SpringDampVector(FCancerSpringVectorState& SpringState,
	const FVector& Current, const FVector& Target, float DeltaTime, float Frequency, float DampingRatio,
	float TargetVelocityAmount)
{
 	return UCanerMathFunctionLibrary::SpringDamp(SpringState, Current, Target, DeltaTime, Frequency, DampingRatio, TargetVelocityAmount);
}

