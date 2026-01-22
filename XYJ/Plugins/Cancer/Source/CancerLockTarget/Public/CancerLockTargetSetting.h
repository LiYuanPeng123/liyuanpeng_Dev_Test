
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Types/LockingDefinition.h"
#include "CancerLockTargetSetting.generated.h"

class UWidgetComponent;

UCLASS(config = Game, DefaultConfig, DisplayName="CancerLocking")
class CANCERLOCKTARGET_API UCancerLockTargetSetting : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere, meta = (DisplayName="默认锁定点UI类"))
	TSoftClassPtr<UUserWidget> DefaultLockingPointWidgetClass;
	
	UPROPERTY(BlueprintReadOnly,EditAnywhere,meta = (DisplayName="默认锁定点UI组件类"))
	TSoftClassPtr<UWidgetComponent> LockingWidgetComponentClass;
	
	UPROPERTY(Config, EditAnywhere, meta = (DisplayName="默认锁定阻挡通道"))
	TEnumAsByte<ETraceTypeQuery>  DefaultLockingBlockTraceType;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认障碍物碰撞类型"))
	TArray<TEnumAsByte<EObjectTypeQuery>> DefaultActionObstacleObjectTypes;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName="锁定设置"))
	FLockingConfigInfo LockingConfig;

	UCancerLockTargetSetting();
	static UCancerLockTargetSetting* Get();

	
	virtual FName GetCategoryName() const;
	virtual FName GetSectionName() const;
};
