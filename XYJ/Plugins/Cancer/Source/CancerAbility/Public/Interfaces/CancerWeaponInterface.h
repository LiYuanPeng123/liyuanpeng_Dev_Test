
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CancerWeaponInterface.generated.h"


UINTERFACE()
class UCancerWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class CANCERABILITY_API ICancerWeaponInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	UMeshComponent* GetWeaponMesh();
	virtual UMeshComponent* GetWeaponMesh_Implementation(){return nullptr;};
};
