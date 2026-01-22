
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CancerArchiveDataAsset.generated.h"

class UCancerArchiveBase;
/**
 * 存档数据
 */
UCLASS()
class CANCERARCHIVE_API UCancerArchiveDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TObjectPtr<UCancerArchiveBase> Archive;

	UFUNCTION(BlueprintCallable, CallInEditor, DisplayName="加载存档")
	void LoadArchiveToDataAsset();

	UFUNCTION(BlueprintCallable, CallInEditor, DisplayName="写入存档")
	void WriteArchiveToFile();
};
