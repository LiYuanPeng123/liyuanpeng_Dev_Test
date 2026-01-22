
#pragma once

#include "CoreMinimal.h"
#include "Types/FBufferedInputCommand.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"

#include "InputBufferInterface.generated.h"

UINTERFACE()
class UInputBufferInterface : public UInterface
{
	GENERATED_BODY()
};


class CANCERINPUT_API IInputBufferInterface
{
	GENERATED_BODY()

	
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input Buffer Interface")
    bool IsInputBufferEnabled() const;
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input Buffer Interface")
    bool IsInputBufferOpen(UPARAM(meta = (Categories = "Input.Buffer.Channel")) FGameplayTag ChannelTag = FGameplayTag()) const;
	
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input Buffer Interface")
    void OpenInputBuffer(UPARAM(meta = (Categories = "Input.Buffer.Channel")) FGameplayTag ChannelTag = FGameplayTag());
	
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input Buffer Interface")
    void BufferInputCommands(TArray<FBufferedInputCommand>& InputCommandsForAction);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input Buffer Interface")
    void CloseInputBuffer(UPARAM(meta = (Categories = "Input.Buffer.Channel")) FGameplayTag ChannelTag = FGameplayTag(),bool bCancelled = false);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input Buffer Interface")
	void ResetInputBuffer();
};
