#pragma once

#include "CancerCameraMode.h"
#include "Curves/CurveFloat.h"
#include "CancerPenetrationAvoidanceFeeler.h"
#include "DrawDebugHelpers.h"
#include "CancerCameraMode_ThirdPerson.generated.h"

class UCancerCameraPenetrationData;
class UCurveVector;

/**
 * UCancerCameraMode_ThirdPerson
 *
 *	A basic third person camera mode.
 */
UCLASS(Abstract, Blueprintable)
class UCancerCameraMode_ThirdPerson : public UCancerCameraMode
{
	GENERATED_BODY()

public:
	UCancerCameraMode_ThirdPerson();

	bool GetLockOnTargetLoc(FVector& TargetPos) const;

protected:
	virtual void UpdateView(float DeltaTime) override;
	virtual void OnActivation() override;

	void UpdatePreventPenetration(float DeltaTime);
	void PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc, FVector& CameraLoc,
	                              float const& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly);
	virtual void DrawDebug(UCanvas* Canvas) const override;

public:
	// === 锁定目标相关 ===
	UPROPERTY(EditAnywhere, Category="锁定", DisplayName="启用锁定")
	bool bEnableLockOn = true;
	UPROPERTY(BlueprintReadWrite, Category="锁定")
	TWeakObjectPtr<AActor> LockOnTarget;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="锁定")
	FName LockingComponentName = FName("组件.网格体");
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="锁定")
	FName LockingSocketName = FName("锁定点");
	
	UPROPERTY(BlueprintReadWrite, Category="锁定")
	bool bEnableLockOnLoc = false;
	UPROPERTY(BlueprintReadWrite, Category="锁定")
	FVector LockOnTargetLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="锁定插值速度",
		meta=(EditCondition="bEnableLockOn"))
	float LockOnInterpSpeed = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="锁定目标偏移",
		meta=(EditCondition="bEnableLockOn"))
	FVector LockOnOffset = FVector(0.f, 0.f, 0.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="锁定插槽偏移垂直",
		meta=(EditCondition="bEnableLockOn"))
	float LockOnOffset_Ver = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="锁定插槽偏移水平",
		meta=(EditCondition="bEnableLockOn"))
	float LockOnOffset_Hor = 100.f;

	// 使用屏幕空间偏移替代世界空间偏移，使目标在屏幕上偏离中心到指定位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="启用屏幕空间偏移",
		meta=(EditCondition="bEnableLockOn"))
	bool bUseScreenSpaceLockOffset = false;
	// 目标在屏幕上的归一化偏移，范围约 [-1, 1]，X 为水平（右为正），Y 为垂直（上为正）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="屏幕空间偏移(归一化)",
		meta=(EditCondition="bEnableLockOn"))
	FVector2D LockOnScreenOffset = FVector2D(0.f, 0.f);

	// 锁定稳定性：预测与角速度限制，减少目标快速位移引起的视角突变
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="启用预测目标位置",
		meta=(EditCondition="bEnableLockOn"))
	bool bLockOnUsePredictiveTarget = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="预测时间(秒)", meta=(ClampMin="0.0"),
		meta=(EditCondition="bEnableLockOn"))
	float LockOnPredictionTime = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="限制角速度",
		meta=(EditCondition="bEnableLockOn"))
	bool bLockOnClampAngularSpeed = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="最大Yaw速度(度/秒)", meta=(ClampMin="0.0"),
		meta=(EditCondition="bEnableLockOn"))
	float LockOnMaxYawSpeed = 180.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="最大Pitch速度(度/秒)", meta=(ClampMin="0.0"),
		meta=(EditCondition="bEnableLockOn"))
	float LockOnMaxPitchSpeed = 180.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="Pitch范围(Min)",
		meta=(EditCondition="bEnableLockOn"))
	float LockOnPitchMin = -60.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="锁定", DisplayName="Pitch范围(Max)",
		meta=(EditCondition="bEnableLockOn"))
	float LockOnPitchMax = 60.f;


	// === 平滑摄像机移动设置 ===
	UPROPERTY(EditAnywhere, Category="迟滞", DisplayName="启用移动插值")
	bool bDoLocationLag = true;

	UPROPERTY(EditAnywhere, Category="迟滞", meta=(ClampMin="0.0"), DisplayName="移动插值速度",
		meta=(EditCondition="bDoLocationLag"))
	float CameraLagSpeed = 6.0f;

	UPROPERTY(EditAnywhere, Category="迟滞", meta=(ClampMin="0.0"), DisplayName="移动插值最大距离",
		meta=(EditCondition="bDoLocationLag"))
	float CameraLagMaxDistance = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="迟滞", DisplayName="启用旋转插值")
	bool bRotLag = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="迟滞", DisplayName="旋转插值速度", meta=(EditCondition="bRotLag"))
	float CameraRotationLagSpeed = 15.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "迟滞", DisplayName="使用摄像机延迟分布")
	uint32 bUseCameraLagSubstepping : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "迟滞", DisplayName="摄像机延迟最大时间步",
		meta=(editcondition = "bUseCameraLagSubstepping", ClampMin="0.005", ClampMax="0.5", UIMin = "0.005", UIMax =
			"0.5"))
	float CameraLagMaxTimeStep = 0.016667;

	/** Temporary variables when using camera lag, to record previous camera position */
	/** 在使用相机延迟时使用的临时变量，用于记录之前的相机位置 */
	FVector PreviousDesiredLoc;
	FVector PreviousArmOrigin;
	/** Temporary variable for lagging camera rotation, for previous rotation */
	/** 用于记录摄像机旋转偏移量的临时变量，用于存储之前的旋转角度 */
	FRotator PreviousDesiredRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "目标", DisplayName="目标偏移")
	FVector TargetOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="目标", DisplayName="插槽偏移")
	FVector SocketOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "目标", DisplayName="摄像机臂长度")
	float TargetArmLength = 321;

	// === 移动回正 ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="移动回正", DisplayName="启用移动回正")
	bool bAutoRecenterWhileMoving = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="移动回正", DisplayName="Yaw范围(Min)",
		meta=(ClampMin="0.0", ClampMax="180.0"))
	float MoveRecenterYawRangeMin = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="移动回正", DisplayName="Yaw范围(Max)",
		meta=(ClampMin="0.0", ClampMax="180.0"))
	float MoveRecenterYawRangeMax = 145.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="移动回正", DisplayName="插值速度", meta=(ClampMin="0.0"))
	float MoveRecenterInterpSpeed = 6.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="移动回正", DisplayName="触发最小移动速度", meta=(ClampMin="0.0"))
	float MoveRecenterMinSpeed2D = 10.f;
	FRotator PreviousLockOnRot;


	// Penetration prevention
public:
	UPROPERTY(BlueprintReadWrite, Category="Collision")
	float PenetrationBlendInTime = 0.1f;

	UPROPERTY(BlueprintReadWrite, Category="Collision")
	float PenetrationBlendOutTime = 0.15f;

	UPROPERTY(BlueprintReadWrite, Category = "Collision")
	float CollisionPushOutDistance = 2.f;

	/** When the camera's distance is pushed into this percentage of its full distance due to penetration */
	/** 当摄像机的拍摄距离因穿透作用而缩短至其最大距离的这一比例时 */
	UPROPERTY(BlueprintReadWrite, Category = "Collision")
	float ReportPenetrationPercent = 0.f;

	/**
	 * These are the feeler rays that are used to find where to place the camera.
	 * Index: 0  : This is the normal feeler we use to prevent collisions.
	 * Index: 1+ : These feelers are used if you bDoPredictiveAvoidance=true, to scan for potential impacts if the player
	 *             were to rotate towards that direction and primitively collide the camera so that it pulls in before
	 *             impacting the occluder.
	 */
	/**
* 这些是用于确定摄像机放置位置的探测光束。
* 编号：0 ：这是我们常用的常规探测光束，用于避免碰撞。
* 编号：1+ ：如果“bDoPredictiveAvoidance=true”，则使用这些探测光束来扫描潜在的碰撞情况，假设玩家会朝那个方向旋转并与摄像机发生初步碰撞，从而在撞击到遮挡物之前将摄像机拉近。*/
	UPROPERTY()
	TArray<FCancerPenetrationAvoidanceFeeler> PenetrationAvoidanceFeelers;

	// Penetration prevention
	// 防止入侵
	UPROPERTY(EditAnywhere, Category = "Collision", DisplayName="碰撞设置")
	UCancerCameraPenetrationData* PenetrationData;

	/** If true, does collision checks to keep the camera out of the world. */
	/** 若为真，则会进行碰撞检测，以确保摄像机不会进入游戏世界。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision", DisplayName="启用碰撞检测")
	bool bPreventPenetration = true;

	/** If true, try to detect nearby walls and move the camera in anticipation.  Helps prevent popping. */
	/** 若为真，则尝试检测附近墙壁并提前移动摄像机。此举有助于避免画面出现抖动现象。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision", DisplayName="碰撞检测(尝试检测附近墙壁并提前移动摄像机)")
	bool bDoPredictiveAvoidance = true;

	UPROPERTY(Transient)
	float AimLineToDesiredPosBlockedPct;

	UPROPERTY(Transient)
	TArray<TObjectPtr<const AActor>> DebugActorsHitDuringCameraPenetration;

#if ENABLE_DRAW_DEBUG
	mutable float LastDrawDebugTime = -MAX_FLT;
#endif
};
