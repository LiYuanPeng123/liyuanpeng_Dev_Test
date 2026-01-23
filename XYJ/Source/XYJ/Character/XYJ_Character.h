#pragma once

#include "CoreMinimal.h"
#include "CancerActionInterface.h"
#include "CancerAITypes.h"
#include "CancerCharacter.h"
#include "Interfaces/CancerCoreSystemInterface.h"
#include "XYJ_Character.generated.h"



UCLASS()
class XYJ_API AXYJ_Character : public ACancerCharacter,public ICancerCoreSystemInterface,public ICancerActionInterface
{
	GENERATED_BODY()

public:
	AXYJ_Character(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PossessedBy(AController* NewController) override;
};
