#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InputBufferInterface.h"
#include "Types/EInputBufferMode.h"
#include "Types/FStoredBufferedInputCommands.h"
#include "CancerInputBufferComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputBuffered, const UInputAction*, BufferedInputAction);

UCLASS(Abstract, Blueprintable, ClassGroup=(CancerInput), meta = (BlueprintSpawnableComponent))
class CANCERINPUT_API UCancerInputBufferComponent : public UActorComponent, public IInputBufferInterface
{
	GENERATED_BODY()

public:
	// 输入缓存事件委托
	UPROPERTY(BlueprintAssignable, Category = "Combo Graph|Events")
	FOnInputBuffered OnInputBuffered;

	UCancerInputBufferComponent();

	// -- Begin Input Buffer Interface
	virtual bool IsInputBufferEnabled_Implementation() const final override;
	virtual bool IsInputBufferOpen_Implementation(FGameplayTag ChannelTag) const final override;
	virtual void BufferInputCommands_Implementation(TArray<FBufferedInputCommand>& InputCommandsForAction) override;
	virtual void OpenInputBuffer_Implementation(FGameplayTag ChannelTag) override;
	virtual void CloseInputBuffer_Implementation(FGameplayTag ChannelTag, bool bCancelled = false) override;
	virtual void ResetInputBuffer_Implementation() override;
	// -- End Input Buffer Interface


	UFUNCTION(BlueprintPure, Category = "Cancer Input|Input Buffer Component")
	UActorComponent* GetInputBufferComponent();

protected:
	UPROPERTY(Transient)
	FGameplayTagContainer Channels;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FStoredBufferedInputCommands> BufferedCommands;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Buffer")
	EInputBufferMode InputBufferMode;

	UFUNCTION(BlueprintPure, Category = "Cancer Input|Input Buffer Component")
	virtual bool CanAddToBuffer(FGameplayTag ChannelTag) const;

private:
	/** Informs the current state of the Input Buffer. */
	UPROPERTY()
	bool bUsingInputBuffer;
};
