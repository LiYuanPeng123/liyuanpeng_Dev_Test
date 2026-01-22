// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CancerCombatDelegates.h"
#include "UObject/Interface.h"
#include "CancerCombatAnimationTaskInterface.generated.h"


UINTERFACE()
class UCancerCombatAnimationTaskInterface : public UInterface
{
	GENERATED_BODY()
};


class CANCERABILITY_API ICancerCombatAnimationTaskInterface
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation Task Interface")
	void BindToAnimationReady(const FCombatAnimationEventDelegate& Delegate);
	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation Task Interface")
	void BindToAnimationStarted(const FCombatAnimationEventDelegate& Delegate);
	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation Task Interface")
	void BindToAnimationCompleted(const FCombatAnimationEventDelegate& Delegate);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation Task Interface")
	void BindToAnimationBlendOut(const FCombatAnimationEventDelegate& Delegate);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation Task Interface")
	void BindToAnimationInterrupted(const FCombatAnimationEventDelegate& Delegate);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation Task Interface")
	void BindToAnimationCancelled(const FCombatAnimationEventDelegate& Delegate);
};
