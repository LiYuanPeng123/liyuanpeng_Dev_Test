// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_InputBuffer.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CancerInputFunctionLibrary.h"
#include "Interfaces/InputBufferInterface.h"

UAnimNotifyState_InputBuffer::UAnimNotifyState_InputBuffer()
{
	BufferChannelTag = FGameplayTag::EmptyTag;

#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(211, 221, 197);
#endif
}

void UAnimNotifyState_InputBuffer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UActorComponent* InputBuffer = UCancerInputFunctionLibrary::GetInputBufferComponent(MeshComp->GetOwner());
	if (IsValid(InputBuffer))
	{
		IInputBufferInterface::Execute_OpenInputBuffer(InputBuffer, BufferChannelTag);
	}
}

void UAnimNotifyState_InputBuffer::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_InputBuffer::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	CloseBuffer(MeshComp->GetOwner());
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

FString UAnimNotifyState_InputBuffer::GetNotifyName_Implementation() const
{
	return "Input Buffer";
}


void UAnimNotifyState_InputBuffer::CloseBuffer(AActor* BufferOwner)
{
	if (!IsValid(BufferOwner))
	{
		return;
	}

	UActorComponent* InputBuffer = UCancerInputFunctionLibrary::GetInputBufferComponent(BufferOwner);
	if (!IsValid(InputBuffer) || !IInputBufferInterface::Execute_IsInputBufferOpen(InputBuffer, BufferChannelTag))
	{
		return;
	}
	IInputBufferInterface::Execute_CloseInputBuffer(InputBuffer, BufferChannelTag, false);
}

#if WITH_EDITOR
bool UAnimNotifyState_InputBuffer::CanBePlaced(UAnimSequenceBase* Animation) const
{
	return Animation && Animation->IsA(UAnimMontage::StaticClass());
}
#endif