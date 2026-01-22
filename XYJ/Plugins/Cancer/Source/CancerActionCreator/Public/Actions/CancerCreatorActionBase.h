#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/SoftObjectPath.h"
#include "CancerCreatorActionBase.generated.h"

class AActor;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class CANCERACTIONCREATOR_API UCancerCreatorActionBase : public UObject
{
	GENERATED_BODY()

public:
	UCancerCreatorActionBase();

	virtual void GatherSoftReferences(TArray<FSoftObjectPath>& OutPaths) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void PostExecuteAction(AActor* InActor);
	virtual void PostExecuteAction_Implementation(AActor* InActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void PreExecuteAction(AActor* InActor);
	virtual void PreExecuteAction_Implementation(AActor* InActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void PreInitComponent();
	virtual void PreInitComponent_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void PostInitComponent();
	virtual void PostInitComponent_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void PostDestroyAction(AActor* InActor);
	virtual void PostDestroyAction_Implementation(AActor* InActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void PreDestroyAction(AActor* InActor);
	virtual void PreDestroyAction_Implementation(AActor* InActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void FinalizeAfterComponent();
	virtual void FinalizeAfterComponent_Implementation();

	UFUNCTION(BlueprintPure, Category = "Action")
	AActor* GetCreatActor() const;

protected:
	TWeakObjectPtr<AActor> CreatActor;
};
