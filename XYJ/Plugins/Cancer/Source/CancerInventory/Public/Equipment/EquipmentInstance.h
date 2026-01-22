#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "EquipmentDefinition.h"
#include "EquipmentInstance.generated.h"


UCLASS(BlueprintType, Blueprintable)
class CANCERINVENTORY_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()

	friend class UCancerQuickBarComponent;

public:
	UEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UWorld* GetWorld() const override final;


	UFUNCTION(BlueprintPure, Category=Equipment)
	UObject* GetInstigator() const { return Instigator; }

	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	UFUNCTION(BlueprintPure, Category=Equipment)
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	UFUNCTION(BlueprintPure, Category=Equipment)
	TArray<AActor*> GetSpawnedActors() const;

	virtual void SpawnEquipmentActors(const TArray<FEquipmentActorToSpawn>& ActorsToSpawn);

	UFUNCTION(BlueprintCallable, Category=Equipment)
	void AttachEquipToSlot(EEquipSocket InEquipSocket);

	UFUNCTION(BlueprintPure, Category=Equipment)
	EEquipSocket GetCurrentEquipSocket() const { return CurrentEquipSocket; }

	virtual void DestroyEquipmentActors();

	void OnEquipped();
	void OnUnequipped();

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnUnequipped"))
	void K2_OnUnequipped();

private:
	UPROPERTY()
	TObjectPtr<UObject> Instigator;

	UPROPERTY()
	EEquipSocket CurrentEquipSocket = EEquipSocket::Default;

	UPROPERTY()
	TArray<FEquipmentInfo> EquipmentInfos;

	UPROPERTY()
	FActiveGameplayEffectHandle EquipBindingEffectHandle;
};
