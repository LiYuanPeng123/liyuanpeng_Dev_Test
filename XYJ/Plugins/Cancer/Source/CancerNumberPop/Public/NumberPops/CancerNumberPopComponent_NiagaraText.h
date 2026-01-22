
#pragma once

#include "CoreMinimal.h"
#include "CancerNumberPopComponent.h"
#include "CancerNumberPopComponent_NiagaraText.generated.h"


class UNiagaraComponent;
class UCancerDamagePopStyleNiagara;

UCLASS(Blueprintable,meta=(BlueprintSpawnableComponent))
class CANCERNUMBERPOP_API UCancerNumberPopComponent_NiagaraText : public UCancerNumberPopComponent
{
	GENERATED_BODY()

public:
	UCancerNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UCancerNumberPopComponent interface
	virtual void AddNumberPop(const FCancerNumberPopRequest& NewRequest) override;
	//~End of UCancerNumberPopComponent interface

	void SetStyle(UCancerDamagePopStyleNiagara* InStyle)
	{
		Style = InStyle;
	};

protected:
	
	TArray<int32> DamageNumberArray;

	/** Style patterns to attempt to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UCancerDamagePopStyleNiagara> Style;

	//Niagara Component used to display the damage
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
};
