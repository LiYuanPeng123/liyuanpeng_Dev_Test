#pragma once

/**
 * Possible states for a pooled actor.
 */
UENUM(BlueprintType)
enum class EPoolableActorState : uint8
{
	Inactive UMETA(DisplayName = "未激活"),
	Reserved UMETA(DisplayName = "等待激活"),
	Active UMETA(DisplayName = "激活"),
};
