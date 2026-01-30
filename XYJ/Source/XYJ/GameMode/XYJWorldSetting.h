
#pragma once

#include "CoreMinimal.h"
#include "Action_DataAssetTemplate.h"
#include "GameFramework/WorldSettings.h"
#include "XYJWorldSetting.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class UPawnData;
/**
 * 
 */
UCLASS()
class XYJ_API AXYJWorldSetting : public AWorldSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XYJ WorldSetting", DisplayName="玩家数据")
	TSoftObjectPtr<UAction_DataAsset> PawnData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XYJ WorldSetting", DisplayName="全局技能")
	TSubclassOf<UGameplayAbility> Abilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XYJ WorldSetting", DisplayName="全局效果")
	TSubclassOf<UGameplayEffect> Effects;
};
