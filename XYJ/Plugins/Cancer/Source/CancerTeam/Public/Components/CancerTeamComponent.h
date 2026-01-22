#pragma once

#include "CoreMinimal.h"
#include "TeamCoreType.h"
#include "UObject/Object.h"
#include "CancerTeamComponent.generated.h"


//团队组件
UCLASS(ClassGroup = (Cancer), Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class CANCERTEAM_API UCancerTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCancerTeamComponent();

	UFUNCTION(BlueprintPure, Category = Cancer)
	bool IsEnemyTeam(FGameplayTag SelfTeam, FGameplayTag TargetTeam, ETeamAttitude::Type TeamAttitude
		                 = ETeamAttitude::Hostile) const;

	UFUNCTION(BlueprintPure, Category = Cancer)
	ETeamAttitude::Type GetAttitude(FGameplayTag SelfTeam, FGameplayTag TargetTeam) const;

	UFUNCTION(BlueprintPure, Category = Cancer)
	FGenericTeamId GetGenericTeamId() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Cancer, DisplayName="阵营标签",meta=(Categories="阵营"))
	FGameplayTag TeamTag;
	
	UFUNCTION(BlueprintPure, Category = Cancer)
	uint8 GetTeamIndex(const FGameplayTag& InTeamTag) const;
	
	UFUNCTION(BlueprintPure, Category = Cancer)
	bool FindTagByIndex(uint8 Index, FGameplayTag& OutTag) const;
	
	void SetTeamTag(FGameplayTag InTag);

protected:
	virtual void BeginPlay() override;
};
