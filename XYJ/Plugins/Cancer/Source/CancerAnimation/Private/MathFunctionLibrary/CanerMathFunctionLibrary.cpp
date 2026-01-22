

#include "MathFunctionLibrary/CanerMathFunctionLibrary.h"

#include "Types/AnimInstanceDefinition.h"


/**
 * 计算浮点数的弹簧阻尼效果
 * 
 * 该函数使用模板函数实现浮点数的弹簧阻尼效果
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
 float UCanerMathFunctionLibrary::SpringDampFloat(FCancerSpringFloatState& SpringState, float Current,
	float Target, float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount)
{
 	return SpringDamp(SpringState, Current, Target, DeltaTime, Frequency, DampingRatio, TargetVelocityAmount);
}


/**
 * 计算运动方向
 * 
 * 该函数根据角度计算运动方向，支持四个基本方向：
 * - 前进（Forward）
 * - 后退（Backward）
 * - 左侧（Left）
 * - 右侧（Right）
 * 
 * @param Angle 当前角度
 * @param ForwardHalfAngle 前向半角
 * @param AngleThreshold 角度阈值
 * @return 运动方向
 * 
 * 该函数使用角度阈值来确定方向，提供了更灵活的方向判断
 */
 ECancerDirection_4 UCanerMathFunctionLibrary::CalculateMovementDirection(float Angle, float ForwardHalfAngle,
	float AngleThreshold)
{
 	// 判断是否在前向范围内
 	if (Angle >= -ForwardHalfAngle - AngleThreshold && Angle <= ForwardHalfAngle + AngleThreshold)
 	{
 		return ECancerDirection_4::Forward;
 	}

 	// 判断是否在右向范围内
 	if (Angle >= ForwardHalfAngle - AngleThreshold && Angle <= 180.0f - ForwardHalfAngle + AngleThreshold)
 	{
 		return ECancerDirection_4::Right;
 	}

 	// 判断是否在左向范围内
 	if (Angle <= -(ForwardHalfAngle - AngleThreshold) && Angle >= -(180.0f - ForwardHalfAngle + AngleThreshold))
 	{
 		return ECancerDirection_4::Left;
 	}

 	// 其他情况为后退
 	return ECancerDirection_4::Backward;
}


/**
 * 计算极向量的位置和方向
 * 
 * 该函数计算从点A到点B的垂直向量，用于IK计算
 * 
 * @param ALocation A点位置
 * @param BLocation B点位置
 * @param CLocation C点位置
 * @param ProjectionLocation 投影位置
 * @param PoleDirection 极向量方向
 * @return 是否计算成功
 * 
 * 该函数主要用于IK计算中的极向量计算，确保骨骼的正确旋转
 */
 bool UCanerMathFunctionLibrary::TryCalculatePoleVector(const FVector& ALocation, const FVector& BLocation,
	const FVector& CLocation, FVector& ProjectionLocation, FVector& PoleDirection)
{
 	// 计算向量AB
 	const auto AbVector{BLocation - ALocation};
 	if (AbVector.IsNearlyZero())
 	{
 		// 如果A和B点重合，无法计算
 		ProjectionLocation = ALocation;
 		PoleDirection = FVector::ZeroVector;
 		return false;
 	}

 	// 计算向量AC并归一化
 	auto AcVector{CLocation - ALocation};
 	if (!AcVector.Normalize())
 	{
 		// 如果AC向量无法归一化，使用默认值
 		ProjectionLocation = ALocation;
 		PoleDirection = AbVector.GetUnsafeNormal(); // A and B are not equal, so normalization will be safe.

 		return true;
 	}

 	// 计算投影点
 	ProjectionLocation = ALocation + AbVector.ProjectOnToNormal(AcVector);
 	PoleDirection = BLocation - ProjectionLocation;

 	return PoleDirection.Normalize(); // Direction will be zero and cannot be normalized if A, B and C are collinear.
}