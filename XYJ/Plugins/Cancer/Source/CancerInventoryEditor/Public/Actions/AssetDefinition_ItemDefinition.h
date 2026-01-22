// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetDefinitionDefault.h"
#include "AssetDefinition_ItemDefinition.generated.h"

/**
 * 
 */
UCLASS()
class CANCERINVENTORYEDITOR_API UAssetDefinition_ItemDefinition : public UAssetDefinitionDefault
{
	GENERATED_BODY()
	public:
	UAssetDefinition_ItemDefinition();

	//----overlay 
	virtual FText GetAssetDisplayName() const override ;
	virtual FLinearColor GetAssetColor() const override ;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	//----overlay 
};
