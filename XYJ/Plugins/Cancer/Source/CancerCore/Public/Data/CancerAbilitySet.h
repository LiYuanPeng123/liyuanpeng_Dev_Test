
#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "Engine/DataAsset.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "CancerAbilitySet.generated.h"

class UGameplayAbility;
class UAttributeSet;
class UGameplayEffect;
class UObject;

UENUM(BlueprintType)
enum class ECancerGrantSource : uint8
{
	DataTables UMETA(DisplayName="数据表格"),
	Arrays     UMETA(DisplayName="数组")
};

USTRUCT(BlueprintType)
struct FCancerAbilitySet_GameplayAbility : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
	
	UPROPERTY(EditDefaultsOnly, DisplayName="技能描述")
	FName DisplayName;
};


USTRUCT(BlueprintType)
struct FCancerAbilitySet_GameplayEffect : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;

	UPROPERTY(EditDefaultsOnly, DisplayName="效果描述")
	FName DisplayName;
};

USTRUCT(BlueprintType)
struct FCancerAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
	UPROPERTY(EditDefaultsOnly, DisplayName="属性描述")
	FName DisplayName;
};


USTRUCT(BlueprintType)
struct FCancerAbilitySet_GameplayAbilityLevel : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,DisplayName="角色等级")
	int32 CharacterLevel = 1;
	UPROPERTY(EditDefaultsOnly)
	TArray<FCancerAbilitySet_GameplayAbility> GameplayAbility;
	UPROPERTY(EditDefaultsOnly)
	TArray<FCancerAbilitySet_GameplayEffect>  GameplayEffect;
};

USTRUCT(BlueprintType)
struct CANCERCORE_API FCancerAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	FCancerAbilitySet_GrantedHandles()
	{
		
	};
	void ClearHandle()
	{
		AbilitySpecHandles.Empty();
		GameplayEffectHandles.Empty();
		GrantedAttributeSets.Empty();
	}
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);
	//移除赋予的技能
	void TakeFromAbilitySystem(UAbilitySystemComponent* CancerASC);

protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};


UCLASS(BlueprintType, Const)
class CANCERCORE_API UCancerAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UCancerAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void GiveToAbilitySystem(UAbilitySystemComponent* InASC, FCancerAbilitySet_GrantedHandles* OutGrantedHandles,
	                         UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable)
	void K2_GiveToAbilitySystem(UAbilitySystemComponent* InASC)
	{
		GiveToAbilitySystem(InASC,nullptr,nullptr);
	}
protected:
	
	// 选择从 DataTables 还是从 Arrays 授予能力与效果
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grant")
	ECancerGrantSource GrantAbilitySource = ECancerGrantSource::DataTables;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,
		DisplayName="GameplayAbilitys", meta=(AllowPrivateAccess = true, Categories="Abilityset",
			RequiredAssetDataTags = "RowStructure=/Script/CancerCore.CancerAbilitySet_GameplayAbility",
			EditCondition="GrantAbilitySource==ECancerGrantSource::DataTables", EditConditionHides))
	TArray<UDataTable*> AbilityDataTables;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="GrantAbilitySource==ECancerGrantSource::Arrays", EditConditionHides))
	TArray<FCancerAbilitySet_GameplayAbility> GameplayAbilitys;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grant")
	ECancerGrantSource GrantEffectSource = ECancerGrantSource::DataTables;
	UPROPERTY(BlueprintReadWrite, EditAnywhere,
		DisplayName="GameplayEffects", meta=(AllowPrivateAccess = true, Categories="Abilityset",
			RequiredAssetDataTags = "RowStructure=/Script/CancerCore.CancerAbilitySet_GameplayEffect",
			EditCondition="GrantEffectSource==ECancerGrantSource::DataTables", EditConditionHides))
	TArray<UDataTable*> EffectDataTables;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="GrantEffectSource==ECancerGrantSource::Arrays", EditConditionHides))
	TArray<FCancerAbilitySet_GameplayEffect> GameplayEffects;

	
	template <class T>
	TArray<T*> GetInfo(const TArray<UDataTable*>& DataTables)
	{
		TArray<T*> InfoArray;

		if (DataTables.IsEmpty())
			return InfoArray;

		for (UDataTable* DataTable : DataTables)
		{
			if (IsValid(DataTable))
			{
				DataTable->GetAllRows(TEXT(""), InfoArray);
			}
		}

		return InfoArray;
	}
public:
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FCancerAbilitySet_AttributeSet> GrantedAttributes;
	
	UPROPERTY(EditAnywhere, Category = "Attribute Sets")
	TObjectPtr<UDataTable> DefaultStartingTable;
};
