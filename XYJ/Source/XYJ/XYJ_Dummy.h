#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XYJ_Dummy.generated.h"

class UPawnData;

UCLASS()
class XYJ_API AXYJ_Dummy : public AActor
{
	GENERATED_BODY()

public:
	AXYJ_Dummy();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPawnData* PawnData;
};
