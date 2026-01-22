#pragma once

#include "CoreMinimal.h"
#include "CancerGeneralFunctionLibrary.generated.h"

namespace ENUMUtility
{
	template <typename TEnum>
	FString GetEnumValueAsString(const FString& Name, TEnum value)
	{
		const UEnum* EnumPtr = StaticEnum<TEnum>();
		if (!EnumPtr)
		{
			return FString("Invalid");
		}
		return EnumPtr->GetNameStringByValue(static_cast<int64>(value));
	}

	template <typename TEnum>
	TEnum GetEnumValueFromString(const FString& Name, FString value)
	{
		const UEnum* EnumPtr = StaticEnum<TEnum>();
		if (!EnumPtr)
		{
			return TEnum(0);
		}
		const int32 Index = EnumPtr->GetIndexByNameString(value);
		if (Index == INDEX_NONE)
		{
			return TEnum(0);
		}
		const int64 EnumValue = EnumPtr->GetValueByIndex(Index);
		return static_cast<TEnum>(EnumValue);
	}

	/*#include "EnumRange.h"*/
	//枚举转int32
	template <typename EnumType> requires std::is_enum_v<EnumType>
	int32 GetIndexByValue(const EnumType Value)
	{
		return StaticEnum<EnumType>()->GetIndexByValue(static_cast<int64>(Value));
	}

	//枚举转FString
	template <typename EnumType> requires std::is_enum_v<EnumType>
	FString GetNameStringByValue(const EnumType Value)
	{
		return StaticEnum<EnumType>()->GetNameStringByValue(static_cast<int64>(Value));
	}
}

UENUM(BlueprintType)
enum class ERHI:uint8
{
	DX11,
	DX12,
	VULKAN
};

UCLASS()
class CANCERGENERAL_API UCancerGeneralFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=General, DisplayName="获取屏幕模式")
	static EWindowMode::Type GetEnumFromString(const FString& Value);

	UFUNCTION(BlueprintCallable, Category=General, DisplayName="设置RHI")
	static void SetRHI(const ERHI& Value);
	UFUNCTION(BlueprintCallable, Category=General, DisplayName="获取RHI")
	static ERHI GetRHI();
};
