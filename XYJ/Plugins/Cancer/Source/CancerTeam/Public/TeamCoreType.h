
#pragma once
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include"TeamCoreType.generated.h"

USTRUCT(BlueprintType)
struct FTeamInfo {
	GENERATED_BODY()

	FTeamInfo()
	{
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Team)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Team)
	TMap<FGameplayTag, TEnumAsByte<ETeamAttitude::Type>> Relationship;
};

/*
UCLASS()
class CANCERTEAM_API UTeamCoreType : public UObject {
	GENERATED_BODY()
};
*/
