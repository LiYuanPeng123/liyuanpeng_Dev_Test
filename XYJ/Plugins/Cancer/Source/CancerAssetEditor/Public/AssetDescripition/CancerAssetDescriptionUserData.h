#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "CancerAssetDescriptionUserData.generated.h"

UCLASS()
class CANCERASSETEDITOR_API UCancerAssetDescriptionUserData : public UAssetUserData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category="Description")
	FString Description;

	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
};
