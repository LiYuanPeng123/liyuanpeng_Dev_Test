
#pragma once

#include "CoreMinimal.h"

#include "CancerAssetFunctionLibrary.generated.h"


class UEditorUtilityWidgetBlueprint;
/**
 * 
 */
UCLASS()
class CANCERASSETEDITOR_API UCancerAssetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	* 检查FString的第一个字符是否是数字
	* @param InString 要检查的字符串
	* @return 如果是数字返回true，否则返回false
	*/
	static bool IsFirstCharDigit(const FString& InString);
	
};


