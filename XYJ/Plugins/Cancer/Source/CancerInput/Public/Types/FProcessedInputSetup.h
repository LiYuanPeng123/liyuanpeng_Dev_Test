// Cancer Bear Studio Inc., all rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "FProcessedBinding.h"
#include "FProcessedInputSetup.generated.h"

class UInputAction;
class UCancerInputConfig;

/**
 * Information about a fully processed Input Setup.
 */
USTRUCT(BlueprintType)
struct FProcessedInputSetup
{
	
	GENERATED_BODY()

	/** Data Asset that has been processed. */
	UPROPERTY()
	TObjectPtr<const UCancerInputConfig> SourceData;

	/** All bindings created for this setup. */
	UPROPERTY()
	TArray<FProcessedBinding> ProcessedBindings;
    
	FProcessedInputSetup()
	{
		SourceData = nullptr;
		ProcessedBindings.Reset();
	}
	
	explicit FProcessedInputSetup(const UCancerInputConfig* SourceData, const TArray<FProcessedBinding>& ProcessedBindings)
		: SourceData(SourceData)
		, ProcessedBindings(ProcessedBindings)
	{
	}
};