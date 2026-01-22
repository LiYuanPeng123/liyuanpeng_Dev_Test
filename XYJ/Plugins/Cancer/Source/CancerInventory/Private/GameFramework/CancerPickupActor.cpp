#include "GameFramework/CancerPickupActor.h"
#include "CancerInputFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CancerInputManagerComponent.h"
#include "Equipment/CancerInteractionDefinition.h"
#include "Kismet/GameplayStatics.h"


ACancerPickupActor::ACancerPickupActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquipMesh"));
	EquipMesh->SetupAttachment(RootComponent);
}

void ACancerPickupActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (EquipDefinition != nullptr && EquipDefinition->DisplayMesh != nullptr)
	{
		EquipMesh->SetStaticMesh(EquipDefinition->DisplayMesh);
		EquipMesh->SetRelativeLocation(EquipDefinition->WeaponMeshOffset);
		EquipMesh->SetRelativeScale3D(EquipDefinition->WeaponMeshScale);
		EquipMesh->SetRelativeRotation(EquipDefinition->WeaponMeshRotation);
	}
}

FGameplayTag ACancerPickupActor::GetPickupItemTag_Implementation()
{
	return PickupItem;
}

void ACancerPickupActor::SetPickupItemTag_Implementation(const FGameplayTag& PickupItemTag)
{
	PickupItem = PickupItemTag;
}

void ACancerPickupActor::SetInteractionDef_Implementation(UCancerInteractionDefinition* InteractionDef)
{
	EquipDefinition = InteractionDef;
}

void ACancerPickupActor::OnTriggerEnter_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	Super::OnTriggerEnter_Implementation(OtherActor, OtherComp);
	if (auto Component = UCancerInputFunctionLibrary::GetCancerInputManagerComponent(OtherActor))
	{
		FOnInputTrigger InputTrigger;
		InputTrigger.BindDynamic(this, &ThisClass::HandleInteraction);
		TriggerHandle = Component->AddTriggerHandle(TriggerAction, TriggerEvent,
		                                            Priority, this, InputTrigger);
		InputManager = Component;
	}
}

void ACancerPickupActor::OnTriggerExit_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	Super::OnTriggerExit_Implementation(OtherActor, OtherComp);
	//取消绑定
	if (auto Component = UCancerInputFunctionLibrary::GetCancerInputManagerComponent(OtherActor))
	{
		if (TriggerHandle.Id.IsValid())
		{
			Component->RemoveTriggerByHandle(TriggerHandle);
			TriggerHandle.Id.Invalidate();
		}
	}
}

void ACancerPickupActor::HandleInteraction_Implementation(FGameplayEventData EventData)
{
	//TODO::大多数情况是由输入直接触发的交互
	//输入触发交互
	Execute_InteractionStart(this, EventData);
}

void ACancerPickupActor::InteractionFinish_Implementation(FGameplayEventData EventData)
{
	AActor* OtherActor = const_cast<AActor*>(EventData.Instigator.Get());
	//取消绑定
	if (auto Component = UCancerInputFunctionLibrary::GetCancerInputManagerComponent(OtherActor))
	{
		if (TriggerHandle.Id.IsValid())
		{
			Component->RemoveTriggerByHandle(TriggerHandle);
			TriggerHandle.Id.Invalidate();
		}
	}
}

void ACancerPickupActor::InteractionStart_Implementation(FGameplayEventData EventData)
{
}

void ACancerPickupActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//取消绑定
	if (InputManager.Get() && TriggerHandle.Id.IsValid())
	{
		InputManager->RemoveTriggerByHandle(TriggerHandle);
		TriggerHandle.Id.Invalidate();
	}
	Super::EndPlay(EndPlayReason);
}

void ACancerPickupActor::K2_FinishInteraction(FGameplayEventData EventData)
{
	Execute_InteractionFinish(this, EventData);
}


void ACancerPickupActor::PlayPickupEffects_Implementation()
{
	if (EquipDefinition != nullptr)
	{
		USoundBase* PickupSound = EquipDefinition->PickedUpSound;
		if (PickupSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}

		UNiagaraSystem* PickupEffect = EquipDefinition->PickedUpEffect;
		if (PickupEffect != nullptr)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, EquipMesh->GetComponentLocation());
		}
	}
}
