#pragma once
#include "Kismet/KismetMathLibrary.h"

#include "AnimInstanceDefinition.generated.h"


USTRUCT(BlueprintType)
struct FCancerLeanAmount
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float LR = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float FB = 0.0f;
};

UENUM(BlueprintType)
enum class ECancerOverlayState:uint8
{
	Default UMETA(DisplayName = "Default"),
	Weak UMETA(DisplayName = "Weak"),
};

USTRUCT(BlueprintType)
struct FCancerAnimConfiguration
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancer|Main Configuration")
	float VelocityBlendInterpSpeed = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancer|Main Configuration")
	float GroundedLeanInterpSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancer|Main Configuration")
	float InAirLeanInterpSpeed = 4.0f;
};

UENUM(Blueprintable, BlueprintType)
enum class ECancerDirection_4:uint8
{
	Forward UMETA(DisplayName = "前"),
	Backward UMETA(DisplayName = "后"),
	Left UMETA(DisplayName = "左"),
	Right UMETA(DisplayName = "右"),
};


UENUM(Blueprintable, BlueprintType)
enum class ECancerDirection_6:uint8
{
	F UMETA(DisplayName = "前"),
	B UMETA(DisplayName = "后"),
	FL UMETA(DisplayName = "前左"),
	FR UMETA(DisplayName = "前右"),
	BL UMETA(DisplayName = "后左"),
	BR UMETA(DisplayName = "后右"),
};


UENUM(Blueprintable, BlueprintType)
enum class ECancerDirection_8:uint8
{
	F UMETA(DisplayName = "前"),
	B UMETA(DisplayName = "后"),
	L UMETA(DisplayName = "左"),
	R UMETA(DisplayName = "右"),
	FL UMETA(DisplayName = "前左"),
	FR UMETA(DisplayName = "前右"),
	BL UMETA(DisplayName = "后左"),
	BR UMETA(DisplayName = "后右"),
};

UENUM(Blueprintable, BlueprintType)
enum class ERootYawOffsetMode:uint8
{
	BlendOut UMETA(DisplayName = "混出"),
	Hold UMETA(DisplayName = "保持"),
	Accumulate UMETA(DisplayName = "累积"),
};


USTRUCT(Blueprintable, BlueprintType)
struct FCancerVelocityBlend
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Velocity", DisplayName="前")
	float VelocityF = 0.f;
	UPROPERTY(BlueprintReadWrite, Category="Velocity", DisplayName="后")
	float VelocityB = 0.f;
	UPROPERTY(BlueprintReadWrite, Category="Velocity", DisplayName="左")
	float VelocityL = 0.f;
	UPROPERTY(BlueprintReadWrite, Category="Velocity", DisplayName="右")
	float VelocityR = 0.f;
};

UENUM(Blueprintable, BlueprintType)
enum class ECancerDirAnim:uint8
{
	Dir_4 UMETA(DisplayName = "4方向"),
	Dir_6 UMETA(DisplayName = "6方向"),
	Dir_8 UMETA(DisplayName = "8方向"),
};

USTRUCT(Blueprintable, BlueprintType)
struct FLocomotionDirection
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="动画选择")
	ECancerDirAnim Mode = {ECancerDirAnim::Dir_4};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> F = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> L = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> R = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> B = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> FL = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> FR = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> BL = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> BR = nullptr;
};


USTRUCT(Blueprintable, BlueprintType)
struct FCancerJumpAnim
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="开始")
	TObjectPtr<UAnimSequenceBase> Jump_Start = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="开始循环")
	TObjectPtr<UAnimSequenceBase> Jump_Start_Loop = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="跳跃至最高点")
	TObjectPtr<UAnimSequenceBase> Jump_Apex = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="下落循L")
	TObjectPtr<UAnimSequenceBase> Jump_Fall_Loop = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="落地")
	TObjectPtr<UAnimSequenceBase> Jump_Fall_Land = nullptr;
};

UENUM(BlueprintType)
enum class ECancerGait : uint8
{
	Walk UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run"),
	Sprint UMETA(DisplayName = "Sprint")
};

UENUM(BlueprintType)
enum class ECustomRotationMode : uint8
{
	EHold UMETA(DisplayName = "保持"),
	EInterpolation UMETA(DisplayName = "插值"),
	EAnimRotation UMETA(DisplayName = "动画旋转"),
};

USTRUCT(BlueprintType)
struct FCustomRotationData
{
	GENERATED_BODY()
	// 是否启用/禁用自定义旋转系统
	UPROPERTY(BlueprintReadOnly)
	bool bUseCustomRotation{};
	// 自定义旋转的偏航角度
	UPROPERTY(BlueprintReadOnly)
	float CustomRotationYaw{};
	// 自定义旋转曲线权重值
	UPROPERTY(BlueprintReadOnly)
	float CustomRotationCurveWeight{};
	// 当前使用的偏航偏移模式
	UPROPERTY(BlueprintReadOnly)
	ECustomRotationMode CustomRotationMode{};
	// 控制器的旋转
	FRotator ControlRotation{};
	// 加速度的旋转
	FRotator AccelerationRotation{};
};

USTRUCT(BlueprintType)
struct FCharacterStates
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ECancerGait Gait{}; // 步态
	UPROPERTY(BlueprintReadOnly)
	bool bOrientToMovement{}; // 自由移动
	UPROPERTY(BlueprintReadOnly)
	bool bHasVelocity{}; // 是否正在移动
	UPROPERTY(BlueprintReadOnly)
	bool bHasAcceleration{}; // 是否有加速度
	UPROPERTY(BlueprintReadOnly)
	bool bIsPivoting{}; // 是否为回转运动
	UPROPERTY(BlueprintReadOnly)
	bool bCrossTurn{}; // 是否为十字交叉转身
	UPROPERTY(BlueprintReadOnly)
	bool bLeftFootUp{}; // 左脚是否抬起
	UPROPERTY(BlueprintReadOnly)
	bool IsOnGround; //是否在地面
	UPROPERTY(BlueprintReadOnly)
	bool IsJumping; //跳跃
	UPROPERTY(BlueprintReadOnly)
	bool bIsDoubleJump; //二段跳跃
	UPROPERTY(BlueprintReadOnly)
	bool IsFalling; //下落
	UPROPERTY(BlueprintReadOnly)
	bool bStand; //站立
	UPROPERTY(BlueprintReadOnly)
	bool IsRunningIntoWall; //是否撞墙
};

USTRUCT(BlueprintType)
struct FCharacterCombatStates
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsOnBlock = false; //是否在格挡中
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWeak = false; //是否在虚弱中
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFallGround = false; //倒地状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsHitInAir = false; //空中受击
};


USTRUCT(BlueprintType)
struct FCharacterRagdoll
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	bool bIsRagdoll; //布娃娃状态
	UPROPERTY(BlueprintReadOnly)
	float FlailRate = 0.0f;
};

USTRUCT(BlueprintType)
struct FCharacterLeanAmount
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FCancerLeanAmount LeanAmount;
	UPROPERTY(BlueprintReadOnly)
	FVector RelativeAccelerationAmount;
};


USTRUCT(BlueprintType)
struct FCharacterMovement
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FCancerVelocityBlend VelocityBlend{FCancerVelocityBlend()}; //速度混合
	UPROPERTY(BlueprintReadOnly)
	float GroundDistance{0.f}; //离地面的距离
	UPROPERTY(BlueprintReadOnly)
	FVector WorldLocation{FVector::ZeroVector}; // 角色的世界位置
	UPROPERTY(BlueprintReadOnly)
	float TimeToJumpApex{0.f}; //跳跃到最高点时间
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity{FVector::ZeroVector}; // 速度向量（3D）
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity2D{FVector::ZeroVector}; // 速度向量（2D，忽略Z轴）
	UPROPERTY(BlueprintReadOnly)
	FVector LocalVelocity2D{FVector::ZeroVector}; // 局部空间中的速度向量（2D）
	UPROPERTY(BlueprintReadOnly)
	FVector Acceleration{FVector::ZeroVector}; // 加速度向量（3D）
	UPROPERTY(BlueprintReadOnly)
	FVector Acceleration2D{FVector::ZeroVector}; // 加速度向量（2D，忽略Z轴）
	UPROPERTY(BlueprintReadOnly)
	FVector LocalAcceleration2D{FVector::ZeroVector}; // 局部空间中的加速度向量（2D）
	UPROPERTY(BlueprintReadOnly)
	FVector LastNonZeroAcceleration{FVector::ZeroVector}; // 最后非零加速度向量：加速度消失前的加速度向量，用于十字交叉转身
	UPROPERTY(BlueprintReadOnly)
	float DisplacementSinceLastUpdate{0.f}; // 帧位移量：两帧的位移差量数据
	UPROPERTY(BlueprintReadOnly)
	float DisplacementSpeed{0.f}; // 帧位移速度
	UPROPERTY(BlueprintReadOnly)
	FVector VelocityAcceleration{FVector::ZeroVector}; //帧速度速度
	UPROPERTY(BlueprintReadOnly)
	float Speed{0.f}; //速度
	UPROPERTY(BlueprintReadOnly)
	float MaxSpeed{0.f}; //最大速度
};

USTRUCT(BlueprintType)
struct FCharacterRotation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float LocalVelocityDirectionAngle{0.f}; //速度方向角度
	UPROPERTY(BlueprintReadOnly)
	float LocalVelocityDirectionAngleWithOffset{0.f}; //速度方向角度 减去RootYawOffset旋转
	UPROPERTY(BlueprintReadOnly)
	float LocalAccelerationAngle{0.f}; //加速度方向角度
	UPROPERTY(BlueprintReadOnly)
	ECancerDirection_4 VelocityDirection{ECancerDirection_4::Forward}; //速度方向
	UPROPERTY(BlueprintReadOnly)
	ECancerDirection_4 VelocityDirectionWithOffset{ECancerDirection_4::Forward}; // 速度方向
	UPROPERTY(BlueprintReadOnly)
	ECancerDirection_4 AccelerationDirection{ECancerDirection_4::Forward}; //加速度方向
	UPROPERTY(BlueprintReadOnly)
	FRotator ActorRotation{FRotator::ZeroRotator}; // 角色当前旋转
	UPROPERTY(BlueprintReadOnly)
	FRotator DesiredRotation{FRotator::ZeroRotator}; // 期望旋转
	UPROPERTY(BlueprintReadOnly)
	FRotator ControlRotation{FRotator::ZeroRotator}; // 控制器旋转
	UPROPERTY(BlueprintReadOnly)
	FRotator AccelerationRotation{FRotator::ZeroRotator}; // 加速度方向旋转
	UPROPERTY(BlueprintReadOnly)
	float YawDeltaSinceLastUpdate{0.f}; // 帧Yaw偏转角度
	UPROPERTY(BlueprintReadOnly)
	float YawDeltaSpeed{0.f}; // 帧Yaw偏转速度
	UPROPERTY(BlueprintReadOnly)
	float CrossTurnAngle{0.f}; // 十字交叉转向角度
	UPROPERTY(BlueprintReadOnly)
	float AimPitch{0.f};
	UPROPERTY(BlueprintReadOnly)
	float RootYawOffset{0.f};
	UPROPERTY(BlueprintReadOnly)
	float AimYaw{0.f};
};

USTRUCT(BlueprintType)
struct FLocomotionConfigData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Config)
	float VelocityBlendInterpSpeed = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Config)
	float GroundedLeanInterpSpeed = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Config)
	UCurveFloat* LeanInAirCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Config)
	bool bEnableRootYawOffset = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Config)
	FFloatSpringState SpringState{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Distance{"Distance"}; // 动画的距离曲线：主要用于距离匹配
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName RotationAlpha{"RotationAlpha"}; // 动画的旋转曲线（0~1）：0表示没有动画旋转数据，1表示动画旋转已经结束
};

USTRUCT(BlueprintType)
struct FLocomotionData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FCharacterStates States{}; // 角色状态数据
	UPROPERTY(BlueprintReadOnly)
	FCharacterMovement Movement{}; // 角色移动数据
	UPROPERTY(BlueprintReadOnly)
	FCharacterRotation Rotation{}; // 角色旋转数据
	UPROPERTY(BlueprintReadOnly)
	FCharacterRagdoll Ragdoll{}; //布娃娃
	UPROPERTY(BlueprintReadOnly)
	FCharacterLeanAmount LeanAmount; //倾斜数据
	UPROPERTY(BlueprintReadOnly)
	bool bFirstUpdate{true}; // 是否是首次更新标志
};
