// Fill out your copyright notice in the Description page of Project Settings.


#include "NumberPops/CancerNumPopFunctionLibrary.h"

#include "NumberPops/CancerNumberPopComponent.h"

UCancerNumberPopComponent* UCancerNumPopFunctionLibrary::K2_GetCancerStaggerComponent(AActor* Actor, bool& bIsValid,
	TSubclassOf<UCancerNumberPopComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerNumPopComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerNumberPopComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}
