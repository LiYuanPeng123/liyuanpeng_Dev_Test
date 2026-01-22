#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncTask_AttributeChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, FGameplayAttribute, Attribute,
                                              AActor*, Instigator, float, NewValue, float, OldValue);

UCLASS(BlueprintType, meta=(ExposedAsyncProxy = AsyncTask))
class CANCERABILITY_API UAsyncTask_AttributeChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged;


	UFUNCTION(BlueprintCallable, Category = "AsyncAction", meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_AttributeChanged* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent,
	                                                             FGameplayAttribute Attribute);


	UFUNCTION(BlueprintCallable, Category = "AsyncAction", meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_AttributeChanged* ListenForAttributesChange(UAbilitySystemComponent* AbilitySystemComponent,
	                                                              TArray<FGameplayAttribute> Attributes);

	UFUNCTION(BlueprintCallable, Category = "AsyncAction")
	void EndTask();

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayAttribute AttributeToListenFor;
	TArray<FGameplayAttribute> AttributesToListenFor;

	void AttributeChanged(const FOnAttributeChangeData& Data);
};
