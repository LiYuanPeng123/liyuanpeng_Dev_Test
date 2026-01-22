#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "Trigger_Base.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ICancerPickupable.h"
#include "Types/FInputTriggerData.h"
#include "CancerPickupActor.generated.h"
class UCancerInteractionDefinition;
class UCancerInputManagerComponent;
class UPickupDefinition;

UCLASS(ClassGroup="Cancer", Abstract)
class CANCERINVENTORY_API ACancerPickupActor : public ATrigger_Base, public ICancerPickupable
{
	GENERATED_BODY()

public:
	ACancerPickupActor(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	//获取交互对象
	virtual FGameplayTag GetPickupItemTag_Implementation() override;

	virtual void SetPickupItemTag_Implementation(const FGameplayTag& PickupItemTag) override;

	virtual void SetInteractionDef_Implementation(UCancerInteractionDefinition* InteractionDef) override;

	//进入交互范围
	virtual void OnTriggerEnter_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp) override;

	//离开交互范围
	virtual void OnTriggerExit_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp) override;

	//交互
	virtual void HandleInteraction_Implementation(FGameplayEventData EventData) override;

	virtual void InteractionFinish_Implementation(FGameplayEventData EventData) override;

	virtual void InteractionStart_Implementation(FGameplayEventData EventData) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void K2_FinishInteraction(FGameplayEventData EventData);

protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Cancer")
	TObjectPtr<UStaticMeshComponent> EquipMesh;

	UFUNCTION(BlueprintNativeEvent, Category = "Cancer")
	void PlayPickupEffects();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancer|Interaction")
	TObjectPtr<UCancerInteractionDefinition> EquipDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Cancer|Interaction")
	TObjectPtr<UInputAction> TriggerAction{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Cancer|Interaction")
	ETriggerEvent TriggerEvent{ETriggerEvent::Triggered};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Cancer|Interaction")
	int32 Priority = -1;

	UPROPERTY()
	TWeakObjectPtr<UCancerInputManagerComponent> InputManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Cancer|Interaction")
	FGameplayTag PickupItem;

private:
	UPROPERTY()
	FInputTriggerHandle TriggerHandle;
};
