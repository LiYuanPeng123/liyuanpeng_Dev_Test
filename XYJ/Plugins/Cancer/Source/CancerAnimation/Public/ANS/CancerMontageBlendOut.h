

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CancerMontageBlendOut.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MontageBlendOutLog, Log, All);

UENUM(BlueprintType)
enum class EBlendOutEnum :uint8
{
	ForceBlendOut UMETA(DisplayName = "强制打断"),
	WithInput UMETA(DisplayName = "移动输入打断"),
	HasInput UMETA(DisplayName = "没有输入打断"),
	Falling UMETA(DisplayName = "下落时打断"),
	HasTag UMETA(DisplayName = "ASC特定标签打断"),
};

UCLASS(DisplayName="蒙太奇打断")
class CANCERANIMATION_API UCancerMontageBlendOut : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend")
	EBlendOutEnum BlendOut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend",meta=(EditCondition = "BlendOut == EBlendOutEnum::HasTag"))
	FGameplayTag BlendOutTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend")
	EMontageBlendMode BlendMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend", meta = (DisplayAfter = "BlendMode"))
	FAlphaBlendArgs Blend;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend", meta = (DisplayAfter = "Blend"))
	FName BlendProfileName;
	

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                        float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;


	virtual FString GetNotifyName_Implementation() const override;
};
