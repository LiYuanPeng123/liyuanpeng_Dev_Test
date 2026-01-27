#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/CancerGameplayAbility.h"
#include "Data/CancerAbilitySet.h"
#include "Data/CancerCombatData.h"
#include "Data/CancerSoulDta.h"
#include "Data/StaggerAnimationData.h"
#include "Types/FCombatAbilitySet.h"
#include "Templates/TypeHash.h"
#include "Types/FCombatAbilitySet.h"
#include "CancerAbilitySystemComponent.generated.h"


class UCancerSpecialAbility;
class UStaggerAnimationDataAsset;
class UChooserTable;
class UCancerWuXueData;
class UCancerCombatData;
class UCancerAbilityTagSetMapping;
class UCancerGameplayAbility;
class UDataTable;

USTRUCT(BlueprintType)
struct FCancerActiveAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer AbilityTag;
	UPROPERTY(BlueprintReadOnly)
	int32 Priority = 100;
	UPROPERTY(BlueprintReadOnly)
	FLinearColor Color = FColor::White;

	FCancerActiveAbilityInfo()
	{
	};

	FCancerActiveAbilityInfo(const FGameplayTagContainer& InAbilityTag, int32 InPriority, FLinearColor InColor)
	{
		AbilityTag = InAbilityTag;
		Priority = InPriority;
		Color = InColor;
	}
};

FORCEINLINE uint32 GetContainerTypeHash(const FGameplayTagContainer& Container)
{
	TArray<FGameplayTag> SortedTags;
	Container.GetGameplayTagArray(SortedTags);

	SortedTags.Sort([](const FGameplayTag& A, const FGameplayTag& B)
	{
		return A.GetTagName().LexicalLess(B.GetTagName());
	});

	uint32 Hash = 0;
	for (const FGameplayTag& Tag : SortedTags)
	{
		Hash = HashCombine(Hash, GetTypeHash(Tag));
	}
	return Hash;
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputPressed, FCancerActiveAbilityInfo, ActiveAbilityInfo);

/*
 * 技能组件
 */
UCLASS(ClassGroup=(Cancer), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class CANCERABILITY_API UCancerAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
#pragma region Debug激活技能优先级查询
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Ability|Priority")
	FOnInputPressed OnInputPressed;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Ability|Priority")
	FOnInputPressed OnAbilityActivate;

	UFUNCTION(BlueprintCallable)
	TArray<FCancerActiveAbilityInfo> GetAllAbilitiesInfo();
#pragma endregion

	UCancerAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	//~UAbilitySystemComponent interface
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual int32 HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual UGameplayAbility*
	CreateNewInstanceOfAbility(FGameplayAbilitySpec& Spec, const UGameplayAbility* Ability) override;
	//~End of UAbilitySystemComponent interface

	void BindToInputManager();

	//获取激活的实例技能
	UFUNCTION(BlueprintCallable)
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer,
	                                TArray<UGameplayAbility*>& ActiveAbilities);
	
	UFUNCTION(BlueprintCallable,DisplayName="AddLooseGameplayTag")
	void K2_AddLooseGameplayTag(FGameplayTag InTag);
	
	UFUNCTION(BlueprintCallable,DisplayName="RemoveLooseGameplayTag")
	void K2_RemoveLooseGameplayTag(FGameplayTag InTag);

#pragma region Tag匹配

	//bExactMatch是否精确匹配
	UFUNCTION(BlueprintCallable)
	bool HasTag(FGameplayTag InTag, bool bExactMatch = true);

	//ASC 所有标签精确匹配
	UFUNCTION(BlueprintCallable)
	bool HasAllTags(const FGameplayTagContainer& InTag);

	//ASC 任一标签精确匹配 bExactMatch是否精确匹配
	UFUNCTION(BlueprintCallable)
	bool HasAnyTags(const FGameplayTagContainer& InTag, bool bExactMatch = true);

	//获取阻挡的Tag
	UFUNCTION(BlueprintCallable)
	const FGameplayTagContainer& GetBlackTag()
	{
		return BlockedAbilityTags.GetExplicitGameplayTags();
	};
	

	UFUNCTION(BlueprintCallable)
	void BlockAbilitiesTags(const FGameplayTagContainer& Tags);
	UFUNCTION(BlueprintCallable)
	void UnBlockAbilitiesTags(const FGameplayTagContainer& Tags);

#pragma endregion

#pragma region 输入
	

	void TryActivateAbilitiesOnSpawn();

	UFUNCTION()
	void AbilityInputTagPressed(const FGameplayTagContainer& InputTagContainer);
	
	UFUNCTION()
	
	void AbilityInputTagReleased(const FGameplayTagContainer& InputTagContainer);
	
	UFUNCTION()
	void AbilityInputTagTriggered(const FGameplayTagContainer& InputTagContainer);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	// 当前帧中已按下输入键所对应的能力的控制柄。
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	//本帧已释放输入的技能句柄。
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	
	// 
	TArray<FGameplayAbilitySpecHandle> InputTriggeredSpecHandles;

#pragma endregion

#pragma region 技能蒙太奇

private:
	UPROPERTY()
	UChooserTable* ChooserTable;

public:
	UFUNCTION(BlueprintCallable, Category="Cancer|Montage")
	UAnimMontage* GetAbilityMontage(const FGameplayTag MontageTag) const;

	UFUNCTION(BlueprintCallable, Category="Cancer|Montage")
	void RefreshAbilityMontageMapFromTables(UChooserTable* InChooserTable);
#pragma endregion

#pragma region 战斗数据资产 气力值等战斗数据

public:
	UFUNCTION(BlueprintCallable, Category="Ability|Combat")
	void SetCombatData(const FCancerSoulData& InCombatData);
	UFUNCTION(BlueprintCallable, Category="Ability|Combat")
	FCancerSoulData GetSoulData() const;


protected:
	UPROPERTY(BlueprintReadOnly)
	FCancerSoulData CombatData;
#pragma	endregion

#pragma region 技能数据 技能优先级 消耗冷却

private:
	UPROPERTY()
	TMap<uint32, FCancerAbilityDataSet> AbilitiesDefMap;
	TArray<FGameplayAbilitySpecHandle> AllAbilitiesHandleArray;

	//运行时的技能优先级变化
	UPROPERTY()
	TMap<uint32, int32> RuntimeTagPriorityOverrides;

public:
	UFUNCTION(BlueprintCallable, Category="Ability|Priority")
	void SetAbilityTagPriorityOverride(const FGameplayTagContainer& AbilityTag, int32 Priority);

	UFUNCTION(BlueprintCallable, Category="Ability|Priority")
	void ClearAbilityTagPriorityOverride();
	
	UFUNCTION(BlueprintCallable, Category="Ability|Priority")
	void ClearAbilityTagPriorityOverrideWithTag(const FGameplayTagContainer& AbilityTag);
	
	UFUNCTION(BlueprintCallable, Category = "Cancer|AbilitySystem")
	bool SetAbilitiesData(UDataTable* InData);

	UFUNCTION(BlueprintCallable, Category = "Cancer|AbilitySystem")
	FGameplayTagContainer GetAbilityTagByInputTag(const FGameplayTag& InInputTag);
	UFUNCTION(BlueprintCallable, Category = "Cancer|AbilitySystem")
	FGameplayTag GetInputTagByAbilityTag(const FGameplayTagContainer& InTagContainer);

	UFUNCTION(BlueprintCallable, Category = "Cancer|AbilitySystem")
	bool GetAbilityIsBlocked(const FGameplayTagContainer& InAbilityTag);

	FCancerAbilityDataSet FindAbilityDataByTags(const FGameplayTagContainer& InAbilityTag) const;

	FCancerAbilityPriority GetAbilityPriority(const FGameplayTagContainer& InAbilityTag, bool& Override);

	UFUNCTION(BlueprintCallable, Category = "Cancer|AbilitySystem")
	bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);
#pragma endregion

#pragma region 技能等级
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "AdvancedGASSystem|AbilitySystem",
		meta = (DisplayName = "SetAbilityLevel"))
	void SetAbilityLevel(FGameplayAbilitySpecHandle SpecHandle, int32 Level = 0);

	UFUNCTION(BlueprintPure, Category = "AdvancedGASSystem|AbilitySystem", meta = (DisplayName = "GetAbilityLevel"))
	int32 GetAbilityLevel(FGameplayAbilitySpecHandle SpecHandle);
#pragma endregion

#pragma region 等级 升级曲线/GE

private:
	UPROPERTY()
	FComboAttributeSet ComboAttributeSet;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> LevelEffect;

public:
	void SetLevelEffect(const TSubclassOf<UGameplayEffect>& InLevelEffect);

	UFUNCTION(BlueprintCallable, Category = "Cancer|AbilitySystem")
	void SetAttributeSetData(const FComboAttributeSet& InComboAttributeSet);

	UFUNCTION(BlueprintCallable, Category = "Cancer|AbilitySystem")
	const FComboAttributeSet& GetAttributeSetData();

	UFUNCTION(BlueprintCallable, Category="Ability|Attribute")
	void UpgradeLevel(int32 DeltalLevel = 1);

	UFUNCTION(BlueprintCallable, Category="Ability|Attribute")
	void SetLevel(int32 Level);

	UFUNCTION(BlueprintCallable, Category="Ability|Attribute")
	float GetLevel();
#pragma endregion

#pragma region 硬质
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cancer|AbilitySystem")
	TObjectPtr<UStaggerAnimationDataAsset> StaggerAnimationData;

	
	UFUNCTION(BlueprintCallable, Category = "Cancer|AbilitySystem")
	FStaggerAnimationConfig GetHitConfig(EStaggerType Module, int32 AnimationID, bool bExactMatch = true);

	UFUNCTION(BlueprintCallable)
	void SetStaggerAnimData(UStaggerAnimationDataAsset* InStaggerAnimationData);
#pragma endregion

#pragma region 特殊技能派生
	UPROPERTY(EditAnywhere,Instanced,Category="Special Abilities")
	UCancerSpecialAbility* SpecialAbility;
	void SetSpecialAbility(UCancerSpecialAbility* InSpecialAbility);
	UCancerSpecialAbility* GetSpecialAbility() const;
	void SetInputTagByAbilityTag(const FGameplayTagContainer& InTagContainer,const FGameplayTag& InputTag);
	UFUNCTION(BlueprintCallable)
	void SetSpecialAbilityData(UDataTable* InSpecialAbilityData);
	FWeaponData* GetSpecialAbilityComboData(const FGameplayTag& InComboTag);
	
protected:
	template <class T>
	TArray<T*> GetInfo(const UDataTable* DataTable)
	{
		TArray<T*> InfoArray;
		if (!DataTable)
			return InfoArray;
		DataTable->GetAllRows(TEXT(""), InfoArray);
		return InfoArray;
	}
	
	TMap<FGameplayTag,FWeaponData> SpecialAbilityData;
#pragma endregion

public:
	// 当命中目标或被命中时  保存攻击双方的引用 便于交互技能确认目标
	// 攻击者
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TWeakObjectPtr<AActor> CurrentHitSource;

	// 受击者
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TWeakObjectPtr<AActor> CurrentHitTarget;

	// 重置目标
	UFUNCTION(BlueprintCallable)
	void ResetHitTargetAndSource();
	
private:
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;
	void OnGameplayEffectApplied(UAbilitySystemComponent* Source, const FGameplayEffectSpec& Spec,
	                             FActiveGameplayEffectHandle Handle);

public:
	TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>>& GetTriggerAbilities();
};
