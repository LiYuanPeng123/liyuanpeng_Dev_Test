#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "Types/AnimInstanceDefinition.h"
#include "CancerAnimInstance.generated.h"

class UInputAction;
class UCharacterMovementComponent;
class UCancerMovementComponent;


/**
 * Cancer AnimInstance
 */
UCLASS()
class CANCERANIMATION_API UCancerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<ACharacter> CharacterPtr;

	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, meta=(BlueprintThreadSafe))
	FGameplayTagContainer GetEquipTag();

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	UCharacterMovementComponent* GetCharacterMovementComponent() const;

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	UCancerMovementComponent* GetCancerCharacterMovementComponent() const;

	virtual void NativeInitializeAnimation() override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly)
	FLocomotionData LocomotionData;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterCombatStates CharacterCombatStates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FLocomotionConfigData LocomotionConfig;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool LinkedLayerChanged;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimInstance* LastLinkedLayer;

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void SetCustomMovementMode(ECustomRotationMode CustomMode);
	
	// 攻击时的IK目标点
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Attack IK")
	FVector AttackIKTargetPoint;

#pragma region GAS Tag綁定

	void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	// Gameplay tags that can be mapped to blueprint variables. The variables will automatically update as the tags are added or removed.
	// These should be used instead of manually querying for the gameplay tags.
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
#pragma endregion

protected:
	FVector CalculateRelativeAccelerationAmount() const;
	FCancerLeanAmount CalculateAirLeanAmount() const;
	UFUNCTION(BlueprintCallable)
	void SetDoubleJump(bool InDoubleJump);
	UFUNCTION(BlueprintCallable)
	void SavePose(const FName SnapshotName); //保存姿势快照
	void SetRootYawOffset(float InRootYawOffset);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Settings")
	FVector2D RootYawOffsetAngleClamp{-120.f, 100.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Settings")
	FVector2D RootYawOffsetAngleClampCrouched{-90.f, 80.f};
};
