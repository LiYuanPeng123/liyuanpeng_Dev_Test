#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "CancerLocomotionComponent.generated.h"

USTRUCT(blueprintType)
struct FLocomotion
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UAnimInstance> AnimInstanceClass;
};

DECLARE_LOG_CATEGORY_EXTERN(CancerLocomotionLog, Log, All);

UCLASS(ClassGroup=(Cancer), meta=(BlueprintSpawnableComponent))
class CANCERANIMATION_API UCancerLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()
	DECLARE_DELEGATE_OneParam(FOnLinkTagChanged, const FGameplayTag&)

public:
	FOnLinkTagChanged OnLinkTagChanged;

	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable)
	void SetCurrentAnimLink(const FGameplayTag& InTag);
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetCurrentAnimLink() const;

protected:
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> OwnerMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="默认动画蓝图类")
	TSubclassOf<UAnimInstance> AnimInstanceClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="默认初始化tag")
	FGameplayTag DefaultLinkTag;

	UPROPERTY(BlueprintReadWrite, DisplayName="当前链接的tag")
	FGameplayTag CurrentLinkTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="链接层")
	TMap<FGameplayTag, FLocomotion> LinkAnimMap;
};
