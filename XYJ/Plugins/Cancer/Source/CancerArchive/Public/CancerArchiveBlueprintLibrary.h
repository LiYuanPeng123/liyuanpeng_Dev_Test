// 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerArchiveBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CANCERARCHIVE_API UCancerArchiveBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="CancerArchive")
	static FString FormatUnixSeconds(int64 UnixSeconds);

	UFUNCTION(BlueprintPure, Category="CancerArchive")
	static FString GetSummarySlotName();
	
	
	UFUNCTION(BlueprintCallable, CustomThunk, Category="CancerArchive|Struct",
		meta=(CustomStructureParam="InStruct", ExpandBoolAsExecs="bIsValid", WorldContext="WorldContextObject"))
	static void SetStructToArchive(const UObject* WorldContextObject, FName Key, const int32& InStruct, bool& bIsValid);
	DECLARE_FUNCTION(execSetStructToArchive);

	UFUNCTION(BlueprintCallable, CustomThunk, Category="CancerArchive|Struct",
		meta=(CustomStructureParam="OutStruct", ExpandBoolAsExecs="bIsValid", WorldContext="WorldContextObject"))
	static void GetStructFromArchive(const UObject* WorldContextObject, FName Key, int32& OutStruct, bool& bIsValid);
	DECLARE_FUNCTION(execGetStructFromArchive);

	UFUNCTION(BlueprintPure, Category="CancerArchive|Struct", meta=(WorldContext="WorldContextObject"))
	static bool ContainsStructInArchive(const UObject* WorldContextObject, FName Key);

	UFUNCTION(BlueprintCallable, Category="CancerArchive|Struct", meta=(WorldContext="WorldContextObject", ExpandBoolAsExecs="bIsValid"))
	static void RemoveStructFromArchive(const UObject* WorldContextObject, FName Key, bool& bIsValid);

	UFUNCTION(BlueprintPure, Category="CancerArchive|Struct", meta=(WorldContext="WorldContextObject"))
	static TArray<FName> GetArchiveStructKeys(const UObject* WorldContextObject);
};
