
#pragma once

#include "CoreMinimal.h"
#include "AnimNotify_RestCombo.generated.h"

/**
 * 
 */
UCLASS()
class CANCERABILITY_API UAnimNotify_RestCombo : public UAnimNotify
{
	GENERATED_BODY()
	public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
