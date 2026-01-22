#pragma once
#include "GameplayTagContainer.h"
#include "EDamageDetectionSource.generated.h"
class UInputAction;


//近战检测的源目标
UENUM(BlueprintType)
enum class EDamageDetectionSource : uint8
{
	/** The Scan or Projectile will originate from the owner's mesh. */
	Owner UMETA(DisplayName ="自身"),

	/** The Scan or Projectile will originate from a weapon's mesh. */
	Weapon UMETA(DisplayName ="武器"),

	/** The Scan or Projectile will originate from a weapon's mesh. */
	Projectile UMETA(DisplayName ="发射物")
};
