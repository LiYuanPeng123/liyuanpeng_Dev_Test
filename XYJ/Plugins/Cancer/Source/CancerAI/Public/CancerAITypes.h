#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CancerAITypes.generated.h"


UENUM(BlueprintType)
enum class EAgentRotation : uint8
{
	EGroupForward = 0 UMETA(DisplayName = "Group Forward"),
	EGroupCenter = 1 UMETA(DisplayName = "Look at Group Center")
};

UENUM(BlueprintType)
enum class EAIState : uint8
{
	EPatrol = 0 UMETA(DisplayName = "寻路"),
	EBattle = 1 UMETA(DisplayName = "战斗"),
	EFollowLeader = 2 UMETA(DisplayName = "跟随目标"),
	EReturnHome = 3 UMETA(DisplayName = "返回初始点"),
	EWait = 4 UMETA(DisplayName = "等待"),
};

UENUM(BlueprintType)
enum class EPatrolType : uint8
{
	EFollowSpline = 0 UMETA(DisplayName = "跟随样条线"),
	ERandomPoint = 1 UMETA(DisplayName = "随机点"),
};


UENUM(BlueprintType)
enum class EAICombatState : uint8
{
	EMeleeCombat = 0 UMETA(DisplayName = "近战攻击"),
	EChaseTarget UMETA(DisplayName = "追踪目标"),
	ERangedCombat UMETA(DisplayName = "远程攻击"),
	EFaceOffTarget UMETA(DisplayName = "对峙"),
	EEludeTarget UMETA(DisplayName = "躲避"),
	EFlee UMETA(DisplayName = "空闲"),
};


UCLASS()
class CANCERAI_API UCancerAITypes : public UObject
{
	GENERATED_BODY()
};
