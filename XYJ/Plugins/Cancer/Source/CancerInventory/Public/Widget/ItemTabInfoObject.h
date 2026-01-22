#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ItemTabInfoObject.generated.h"

UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType)
class UItemTabInfoObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="标签过滤")
	FGameplayTag TabItemQuery;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="显示图片")
	TSoftObjectPtr<UTexture2D> TabItemTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="选择图片")
	TSoftObjectPtr<UTexture2D> SelectedTabItemTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="显示名称")
	FName TabItemTet;
};
