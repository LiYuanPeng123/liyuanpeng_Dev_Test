#pragma once
#include "AssetActionUtility.h"
#include "GameplayTagContainer.h"
#include "CancerEditorLibrary.generated.h"


USTRUCT( BlueprintType)
struct FCancerGameplayTagRedirect
{
	GENERATED_BODY()

public:
	FCancerGameplayTagRedirect() { }

	UPROPERTY(EditAnywhere, Category = GameplayTags)
	FName OldName;

	UPROPERTY(EditAnywhere, Category = GameplayTags)
	FName NewName;

	UPROPERTY(EditAnywhere, Category = GameplayTags)
	bool bDeleteOldTag = false;

	friend inline bool operator==(const FCancerGameplayTagRedirect& A, const FCancerGameplayTagRedirect& B)
	{
		return A.OldName == B.OldName && A.NewName == B.NewName;
	}

	// This enables lookups by old tag name via FindByKey
	bool operator==(FName OtherOldTagName) const
	{
		return OldName == OtherOldTagName;
	}
};

USTRUCT(BlueprintType)
struct FCancerGameplayTagRedirectContainer
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = GameplayTags)
	TArray<FCancerGameplayTagRedirect> Redirects;
};

UCLASS()
class CANCEREDITOR_API UCancerEditorLibrary:public UBlueprintFunctionLibrary
{
public:
	GENERATED_BODY()
	public:
	UFUNCTION(CallInEditor, DisplayName="重定向Tag")
	static void Edit_RedirectTag(const TArray<FCancerGameplayTagRedirect>& Redirects);
	static void RemoveTag(FGameplayTag InTag);
	UFUNCTION(CallInEditor, DisplayName="重启编辑器")
	static void RequestRestartEditor();
};
