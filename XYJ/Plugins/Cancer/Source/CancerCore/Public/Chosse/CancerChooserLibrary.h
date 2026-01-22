#pragma once

#include "CoreMinimal.h"
#include "ChooserFunctionLibrary.h"
#include "FChooserType.h"
#include "IObjectChooser.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerChooserLibrary.generated.h"

class UChooserTable;

UCLASS()
class CANCERCORE_API UCancerChooserLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 
	 * C++ Template for getting animation from chooser with a specific struct type.
	 * This preserves compatibility with existing C++ code like CancerAbilitySystemComponent.
	 */
	template <typename T = UAnimSequenceBase, typename TStruct>
	static T* GetAnimFromChooser(AActor* TargetActor, const TStruct& ChooserData, UChooserTable* ChooserTable)
	{
		if (!ChooserTable)
		{
			return nullptr;
		}

		FChooserEvaluationContext EvalCtx = UChooserFunctionLibrary::MakeChooserEvaluationContext();

		// Add the struct parameter (TStruct is inferred as the struct type, e.g. FChooserType)
		// We make a local copy to ensure we have mutable memory for FStructView if needed by internal storage
		TStruct MutableData = ChooserData;
		EvalCtx.AddStructParam(MutableData);

		if (TargetActor)
		{
			EvalCtx.AddObjectParam(TargetActor);
		}

		FInstancedStruct EvaluateStruct = UChooserFunctionLibrary::MakeEvaluateChooser(ChooserTable);

		UObject* ResultObj = UChooserFunctionLibrary::EvaluateObjectChooserBase(
			EvalCtx,
			EvaluateStruct,
			T::StaticClass(),
			/*bResultIsClass*/ false
		);
		return Cast<T>(ResultObj);
	};

	// Helper function for the CustomThunk implementation
	static UAnimSequenceBase* Generic_GetAnimFromChooser(AActor* TargetActor, const FInstancedStruct& ChooserData, UChooserTable* ChooserTable, bool& bIsValid, TSubclassOf<UAnimSequenceBase> OutputClass);

	/**
	 * Blueprint CustomThunk function allowing any struct to be passed (Wildcard).
	 * Mapped to "GetAnimFromChooser" in Blueprint.
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Cancer", DisplayName="GetAnimFromChooser",
		meta=(CustomStructureParam = "ChooserStruct", DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static UAnimSequenceBase* GetAnimFromChooserBP(AActor* TargetActor, const int32& ChooserStruct, UChooserTable* ChooserTable, bool& bIsValid,
										  TSubclassOf<UAnimSequenceBase> OutputClass = nullptr);
	
	DECLARE_FUNCTION(execGetAnimFromChooserBP);
};
