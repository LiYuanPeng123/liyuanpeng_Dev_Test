#include "Components/CancerInputBufferComponent.h"
#include "CancerInputFunctionLibrary.h"
#include "CancerInputLog.h"
#include "CancerInputHandler.h"

UE_DISABLE_OPTIMIZATION

UCancerInputBufferComponent::UCancerInputBufferComponent()
{
	InputBufferMode = EInputBufferMode::LastCommand;
	bUsingInputBuffer = false;
}

bool UCancerInputBufferComponent::IsInputBufferEnabled_Implementation() const
{
	return InputBufferMode != EInputBufferMode::Disabled;
}

bool UCancerInputBufferComponent::IsInputBufferOpen_Implementation(FGameplayTag ChannelTag) const
{
	return Channels.HasTagExact(ChannelTag);
}

void UCancerInputBufferComponent::OpenInputBuffer_Implementation(FGameplayTag ChannelTag)
{
	if (!Execute_IsInputBufferOpen(this, ChannelTag))
	{
		Channels.AddTagFast(ChannelTag);

		UE_LOG(CancerInputLog, Verbose, TEXT("[%s] Buffer channel %s is open."),
		       *GetNameSafe(GetOwner()), *ChannelTag.ToString());
	}
}

void UCancerInputBufferComponent::CloseInputBuffer_Implementation(FGameplayTag ChannelTag, bool bCancelled)
{
	if (Execute_IsInputBufferOpen(this, ChannelTag))
	{
		if (bCancelled)
		{
			UE_LOG(CancerInputLog, Warning, TEXT("[%s] Discarding buffer channel %s since it was cancelled."),
			       *GetNameSafe(GetOwner()), *ChannelTag.ToString());
		}
		else
		{
			FStoredBufferedInputCommands* Commands = BufferedCommands.Find(ChannelTag);
			if (Commands && Commands->IsValid())
			{
				for (const FBufferedInputCommand& Command : Commands->InputCommandsForAction)
				{
					if (Command.IsValid())
					{
						const UInputAction* InputAction = Command.GetSourceAction();
						const UCancerInputHandler* Handler = Command.GetHandler();
						OnInputBuffered.Broadcast(InputAction);
						UE_LOG(CancerInputLog, Warning,
						       TEXT("[%s] Releasing Input Action %s and Handler %s from buffer channel %s."),
						       *GetNameSafe(GetOwner()), *GetNameSafe(InputAction), *GetNameSafe(Handler),
						       *ChannelTag.ToString());

						Command.Execute();
					}
				}
			}
		}
	}

	Channels.RemoveTag(ChannelTag);
	BufferedCommands.Remove(ChannelTag);
}

void UCancerInputBufferComponent::ResetInputBuffer_Implementation()
{
	Channels.Reset();
	BufferedCommands.Empty();
}

void UCancerInputBufferComponent::BufferInputCommands_Implementation(
	TArray<FBufferedInputCommand>& InputCommandsForAction)
{
	const TArray<FBufferedInputCommand>& ValidCommands = InputCommandsForAction.FilterByPredicate(
		[](const FBufferedInputCommand& Command)
		{
			return Command.IsValid();
		});

	if (ValidCommands.IsEmpty())
	{
		// No commands to buffer, all entries were invalid.
		return;
	}

	const FGameplayTag BufferChannelTag = InputCommandsForAction[0].GetHandler()->GetBufferChannelTag();
	if (!BufferChannelTag.IsValid() || !CanAddToBuffer(BufferChannelTag))
	{
		// Invalid channel tag or buffer request.
		return;
	}

	const FStoredBufferedInputCommands& StoredCommands = FStoredBufferedInputCommands(ValidCommands);
	if (StoredCommands.IsValid())
	{
		BufferedCommands.Add(BufferChannelTag, StoredCommands);

		UE_LOG(CancerInputLog, Verbose, TEXT("[%s] Action %s added %d Handlers to the Input Buffer."),
		       *GetNameSafe(GetOwner()), *GetNameSafe(StoredCommands.InputAction), StoredCommands.Num());
	}
	else
	{
		UE_LOG(CancerInputLog, Warning, TEXT("[%s] Action %s generated an invalid saved command!"),
		       *GetNameSafe(GetOwner()), *GetNameSafe(StoredCommands.InputAction));
	}
}

UActorComponent* UCancerInputBufferComponent::GetInputBufferComponent()
{
	// If this buffer is enabled, then there's no need to perform the look-up.
	if (Execute_IsInputBufferEnabled(this))
	{
		return Cast<UActorComponent>(this);
	}

	// Try to find another buffer that's currently active.
	return UCancerInputFunctionLibrary::GetInputBufferComponent(GetOwner());
}

bool UCancerInputBufferComponent::CanAddToBuffer(FGameplayTag ChannelTag) const
{
	return InputBufferMode == EInputBufferMode::LastCommand
		|| (InputBufferMode == EInputBufferMode::FirstCommand && BufferedCommands.Contains(ChannelTag));
}

UE_ENABLE_OPTIMIZATION
