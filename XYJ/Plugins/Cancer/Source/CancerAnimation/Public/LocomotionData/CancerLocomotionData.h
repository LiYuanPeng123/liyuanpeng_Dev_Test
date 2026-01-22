#pragma once
#include "Types/AnimInstanceDefinition.h"

#include "CancerLocomotionData.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(CancerLocomotionDataLog, Log, All);

struct FGameplayTag;

USTRUCT(BlueprintType)
struct CANCERANIMATION_API FCancerMontageSet
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float StartPosition = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float PlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName StartingSection = NAME_None;
};


USTRUCT(BlueprintType)
struct CANCERANIMATION_API FGaitTransition
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Walk过渡到Run")
	TObjectPtr<UAnimSequenceBase> WalkToRun;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Walk过渡到Sprint")
	TObjectPtr<UAnimSequenceBase> WalkToSprint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Run过渡到Walk")
	TObjectPtr<UAnimSequenceBase> RunToSprint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Run过渡到Sprint")
	TObjectPtr<UAnimSequenceBase> RunToWalk;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Sprint过渡到Walk")
	TObjectPtr<UAnimSequenceBase> SprintToWalk;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Sprint过渡到Run")
	TObjectPtr<UAnimSequenceBase> SprintToSprint;
};


USTRUCT(BlueprintType)
struct CANCERANIMATION_API FTurnPivot
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="TurnPivotR_180")
	FCancerMontageSet TurnPivotR_180;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="TurnPivotL_180")
	FCancerMontageSet TurnPivotL_180;
};

//起步动画
USTRUCT(Blueprintable, BlueprintType)
struct CANCERANIMATION_API FStartAnim
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="F")
	TObjectPtr<UAnimSequenceBase> F;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="FL-90")
	TObjectPtr<UAnimSequenceBase> FL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="FR-90")
	TObjectPtr<UAnimSequenceBase> FR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="FL-180")
	TObjectPtr<UAnimSequenceBase> BL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="FR-180")
	TObjectPtr<UAnimSequenceBase> BR;
};


USTRUCT(BlueprintType)
struct CANCERANIMATION_API FOrientMovementSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stance", DisplayName="Loop循环")
	TObjectPtr<UAnimSequenceBase> IdleAnim = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Crouch", DisplayName="C_Loop循环")
	TObjectPtr<UAnimSequenceBase> CrouchAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stance", DisplayName="Walk起步")
	FStartAnim Start_Anim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stance", DisplayName="Run起步")
	FStartAnim Start_RunAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stance", DisplayName="Sprint起步")
	FStartAnim Start_SprintAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Crouch", DisplayName="C_Walk起步")
	FStartAnim C_Start_Anim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Crouch", DisplayName="C_Run起步")
	FStartAnim C_Start_RunAnim;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stance", DisplayName="Walk循环")
	FLocomotionDirection Cycle_Anim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stance", DisplayName="Run循环")
	FLocomotionDirection Cycle_RunAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stance", DisplayName="Sprint循环")
	FLocomotionDirection Cycle_SprintAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Crouch", DisplayName="C_Walk循环")
	FLocomotionDirection C_Cycle_Anim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Crouch", DisplayName="C_Run循环")
	FLocomotionDirection C_Cycle_RunAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stance", DisplayName="Walk停步")
	TObjectPtr<UAnimSequenceBase> Stop_Anim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stance", DisplayName="Run停步")
	TObjectPtr<UAnimSequenceBase> Stop_RunAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stance", DisplayName="Sprint停步")
	TObjectPtr<UAnimSequenceBase> Stop_SprintAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Crouch", DisplayName="C_Walk停步")
	TObjectPtr<UAnimSequenceBase> C_Stop_Anim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Crouch", DisplayName="C_Run停步")
	TObjectPtr<UAnimSequenceBase> C_Stop_RunAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="跳跃", DisplayName="区分跳跃步态")
	bool bSelectJump = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="跳跃", DisplayName="Walk跳跃")
	FCancerJumpAnim JumpAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="跳跃", DisplayName="Run跳跃",
		meta=(EditCondition="bSelectJump", EditConditionHides))
	FCancerJumpAnim RunJumpAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="跳跃", DisplayName="Sprint跳跃",
		meta=(EditCondition="bSelectJump", EditConditionHides))
	FCancerJumpAnim SprintJumpAnim;
};


USTRUCT(BlueprintType)
struct CANCERANIMATION_API FStrafeStateStanceLocomotion
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Ilde循环")
	TObjectPtr<UAnimSequenceBase> IdleAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Walk起步")
	FLocomotionDirection Start_Anim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Run起步")
	FLocomotionDirection Start_RunAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Sprint起步")
	FLocomotionDirection Start_SprintAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Walk循环")
	FLocomotionDirection Cycle_Anim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Run循环")
	FLocomotionDirection Cycle_RunAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Sprint循环")
	FLocomotionDirection Cycle_SprintAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Walk停步")
	FLocomotionDirection Stop_Anim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Run停步")
	FLocomotionDirection Stop_RunAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="Sprint停步")
	FLocomotionDirection Stop_SprintAnim;
};

USTRUCT(BlueprintType)
struct CANCERANIMATION_API FStrafeStateLocomotion
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="站立")
	FStrafeStateStanceLocomotion StrafeStateStanceLocomotion;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="蹲伏")
	FStrafeStateStanceLocomotion StrafeStateCrouchLocomotion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="跳跃")
	FCancerJumpAnim JumpAnim;
};


UCLASS()
class CANCERANIMATION_API UCancerOrientMovementLocomotionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="自由移动动画", meta=(BlueprintThreadSafe))
	FOrientMovementSet FreeLocomotionAnim;

	UAnimSequenceBase* GetStanceAnim();

	UAnimSequenceBase* GetCrouchAnim();

	FStartAnim GetStartAnim(const FGameplayTag& GaitTag, bool bStance);

	FLocomotionDirection GetCycleAnim(const FGameplayTag& GaitTag, bool bStance);

	UAnimSequenceBase* GetStopAnim(const FGameplayTag& GaitTag, bool bStance);

	FCancerJumpAnim GetJumpAnim(const FGameplayTag& GaitTag);
};

UCLASS()
class CANCERANIMATION_API UCancerStrafeMovementLocomotionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="锁定移动动画", meta=(BlueprintThreadSafe))
	FStrafeStateLocomotion StrafeStateLocomotion;

	UAnimSequenceBase* GetStanceAnim();

	UAnimSequenceBase* GetCrouchAnim();

	FLocomotionDirection GetStartAnim(const FGameplayTag& GaitTag, bool bStance);

	FLocomotionDirection GetCycleAnim(const FGameplayTag& GaitTag, bool bStance);

	FLocomotionDirection GetStopAnim(const FGameplayTag& GaitTag, bool bStance);

	FCancerJumpAnim GetJumpAnim(const FGameplayTag& GaitTag);
};


UCLASS()
class CANCERANIMATION_API UCancerLocomotionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="自由移动动画")
	UCancerOrientMovementLocomotionData* OrientLocomotionData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="移动动画")
	UCancerStrafeMovementLocomotionData* StrafeLocomotionData;
	

	
};
