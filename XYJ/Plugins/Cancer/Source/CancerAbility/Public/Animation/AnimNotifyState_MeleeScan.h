
#pragma once

#include "CoreMinimal.h"
#include "Animation/CancerCombatAnimNotifyState.h"
#include "GameFramework/CancerMeleeScan.h"
#include "AnimNotifyState_MeleeScan.generated.h"

class UCancerDamageType;
/**
 * 
 */
UCLASS(DisplayName="近战扫描")
class CANCERABILITY_API UAnimNotifyState_MeleeScan : public UCancerCombatAnimNotifyState
{
	GENERATED_BODY()
public:
	UAnimNotifyState_MeleeScan();
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Instanced,Category = "Melee Scan",DisplayName="伤害")
	TArray<TObjectPtr<UCancerDamageType>> Damages;

	UPROPERTY()
	TArray<UCancerMeleeScan*> MeleeScans;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<int32> RequestIds;
	
	// -- Begin Notify State implementation
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	// -- End Notify State implementation

	virtual void HandleTimeExpired(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Melee Scan")
	UCancerMeleeScan* CreateMeleeScanInstance(AActor* Owner, AActor* Causer,UCancerDamageType *DamageType,
		UMeshComponent* SourceMesh, float Duration) const;
	
	virtual FString GetNotifyName_Implementation() const override;
};
