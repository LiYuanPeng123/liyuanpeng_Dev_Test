// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "ActionDef.generated.h"


USTRUCT()
struct FCancerHUDElementEntry
{
	GENERATED_BODY()
	// The widget to spawn
	UPROPERTY(EditAnywhere, Category=UI)
	TSoftClassPtr<UUserWidget> WidgetClass;
	// The slot ID where we should place this widget
	UPROPERTY(EditAnywhere, Category = UI)
	FGameplayTag SlotID;
};


USTRUCT(BlueprintType)
struct FItemSet : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemSet", meta=(Categories = "道具"))
	FGameplayTag ItemTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemSet", meta=(ClampMin="0"))
	int32 ItemNumber = 1;
};


USTRUCT(BlueprintType)
struct FEquipSet : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemSet", meta=(Categories = "道具"))
	FGameplayTag ItemTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemSet", meta=(ClampMin="0"))
	int32 ItemNumber = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemSet", meta=(Categories = "快捷栏"))
	FGameplayTag QuickBarTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemSet", meta=(ClampMin="0"))
	int32 SlotID = 0;
};