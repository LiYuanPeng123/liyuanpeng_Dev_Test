
#pragma once

#include "CoreMinimal.h"
#include "Action_DataAssetTemplate.h"
#include "GameFramework/WorldSettings.h"
#include "XYJWorldSetting.generated.h"

class UPawnData;
/**
 * 
 */
UCLASS()
class XYJ_API AXYJWorldSetting : public AWorldSettings
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XYJ WorldSetting")
	TSoftObjectPtr<UAction_DataAsset> PawnData;
};
