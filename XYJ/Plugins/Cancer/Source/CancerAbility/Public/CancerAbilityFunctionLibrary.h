#pragma once

#include "CoreMinimal.h"
#include "CancerAbilitySystemComponent.h"
#include "AbilitySystem/Effects/CancerEffect_Damage.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/Action/FWCountdownAction.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CancerAbilityFunctionLibrary.generated.h"

class UCancerEffect_Soul;
class UCancerCombatAttributeComponent;
class UCancerCosmeticsComponent;
struct FCancerComboInfo;
class UCancerDamageType;
class UCancerStaggerComponent;
class UCancerEffect_LevelSet;
class UCancerEffect_Cure;
class UCancerEffect_Damage;
class UCancerHeroComponent;
class UCancerMotionWarpingComponent;

USTRUCT(BlueprintType)
struct FAttributeInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName AttributeName = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Value = 0.0f;

	FAttributeInfo()
	{
	}

	FAttributeInfo(FName InAttributeName, float InValue): AttributeName(InAttributeName), Value(InValue)
	{
	}
};

UENUM(BlueprintType)
enum class EGAModeState:uint8
{
	Active UMETA(DisplayName = "Active"),

	Blocked UMETA(DisplayName = "Blocked"),

	NoActive UMETA(DisplayName = "NoActive")
};

USTRUCT(BlueprintType)
struct FAbilityInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName AbilityName = FName();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EGAModeState State = EGAModeState::Active;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString BlockMessage = FString();

	FAbilityInfo()
	{
	}
};




/**
 * 
 */
UCLASS()
class CANCERABILITY_API UCancerAbilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
#pragma  region 获取 UCancerAbilitySystemComponent
	template <class T = UCancerAbilitySystemComponent>
	static T* GetCancerAbilitySystemComponent(AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;
		return Actor->GetComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Cancer", DisplayName="Get Cancer Abilitysystem Component",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static UCancerAbilitySystemComponent* K2_GetCancerAbilitySystemComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerAbilitySystemComponent> OutputClass = nullptr);
#pragma endregion

#pragma  region 获取 UCancerMotionWarpingComponent
	template <class T = UCancerMotionWarpingComponent>
	static T* GetCancerMotionWarpingComponent(AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;
		return Actor->GetComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Cancer", DisplayName="Get Cancer MotionWarping Component",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static UCancerMotionWarpingComponent* K2_GetCancerMotionWarpingComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerMotionWarpingComponent> OutputClass = nullptr);
#pragma endregion

#pragma  region 获取 UCancerHeroComponent
	template <class T = UCancerHeroComponent>
	static T* GetCancerCombatManagerComponent(AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;
		return Actor->GetComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Cancer", DisplayName="Get Cancer Combat Manager",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static UCancerHeroComponent* K2_GetCancerCombatManagerComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerHeroComponent> OutputClass = nullptr);
#pragma endregion

#pragma  region 获取 UCancerCosmeticsComponent
	template <class T = UCancerCosmeticsComponent>
	static T* GetCancerCosmeticsComponent(AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;
		return Actor->GetComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Cancer", DisplayName="GetCancerCosmeticsComponent",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static UCancerCosmeticsComponent* K2_GetCancerCosmeticsComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerCosmeticsComponent> OutputClass = nullptr);
#pragma endregion

#pragma  region 获取 UCancerCombatAttributeComponent
	template <class T = UCancerCombatAttributeComponent>
	static T* GetCancerCombatAttributeComponent(AActor* Actor)
	{
		if (!IsValid(Actor)) return nullptr;
		return Actor->GetComponentByClass<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Cancer", DisplayName="GetCancerCombatAttributeComponent",
		meta=(DeterminesOutputType= "OutputClass", ExpandBoolAsExecs="bIsValid"))
	static UCancerCombatAttributeComponent* K2_GetCancerCombatAttributeComponent(
		AActor* Actor, bool& bIsValid, TSubclassOf<UCancerCombatAttributeComponent> OutputClass = nullptr);
#pragma endregion

#pragma region GE效果相关
	//施加伤害
	UFUNCTION(BlueprintCallable, Category="Cancer|Damage")
	static bool K2_ApplyDamage_SetByCaller(
		AActor* Target,
		const UCancerDamageType* DamageObject,
		FGameplayTag OptionalDamageAssetTag,
		float DamageCoeff = 1,
		float SoulDamageCoeff = 1,
		FVector SoulValue3 = FVector(1, 1, 1),
		TSubclassOf<UCancerEffect_Damage> DamageGE = nullptr
	);

	// 持续伤害（返回GE上下文）：在施加伤害时额外输出FGameplayEffectContextHandle
	UFUNCTION(BlueprintCallable, Category="Cancer|Damage", meta=(ExpandBoolAsExecs="Return"))
	static FActiveGameplayEffectHandle K2_ApplyDamage_SetByCaller_WithContext(
		AActor* Target,
		FGameplayTag OptionalDamageAssetTag,
		UCancerDamageType* DamageObject,
		bool& Return,
		FGameplayEffectContextHandle& OutContext,
		float DamageCoeff = 1,
		float SoulDamageCoeff = 1,
		FVector SoulValue3 = FVector(1, 1, 1),
		TSubclassOf<UCancerEffect_Damage> DamageGE = nullptr

	);
	//治疗
	UFUNCTION(BlueprintCallable, Category="Cancer|Cure", meta=(ExpandBoolAsExecs="Return"))
	static FActiveGameplayEffectHandle K2_ApplyCure_SetByCaller(
		AActor* Target,
		FGameplayTag OptionalCureAssetTag,
		bool& Return,
		float Cure = 10,
		TSubclassOf<UCancerEffect_Cure> CureGE = nullptr
	);

	// 治疗（返回GE上下文）：在施加治疗时额外输出FGameplayEffectContextHandle
	UFUNCTION(BlueprintCallable, Category="Cancer|Cure", meta=(ExpandBoolAsExecs="Return"))
	static FActiveGameplayEffectHandle K2_ApplyCure_SetByCaller_WithContext(
		AActor* Target,
		FGameplayTag OptionalCureAssetTag,
		bool& Return,
		FGameplayEffectContextHandle& OutContext,
		float Cure = 10,
		TSubclassOf<UCancerEffect_Cure> CureGE = nullptr

	);
	
	// 通用GE
	UFUNCTION(BlueprintCallable, Category="Cancer|Cure")
	static bool K2_ApplyGeneralGE_Instant(AActor* Source,
		AActor* Target,
		FGameplayTag PropertyTag,
		float Value
	);


	//设置等级
	UFUNCTION(BlueprintCallable, Category="Cancer|Level", meta=(ExpandBoolAsExecs="Return"))
	static FActiveGameplayEffectHandle K2_ApplyLevel_SetByCaller(
		AActor* Target,
		FGameplayTag OptionalLevelAssetTag,
		bool& Return,
		int32 Level = 1,
		float Exp = 0,
		TSubclassOf<UCancerEffect_LevelSet> LevelGE = nullptr
	);


	// 施加霸体效果，并指定霸体等级与持续时间 
	UFUNCTION(BlueprintCallable, Category="Cancer|IronBody")
	static bool K2_ApplyIronBody(
		AActor* Target,
		int32 IronBodyLevel = 1,
		TSubclassOf<class UCancerEffect_IronBody> IronBodyGE = nullptr
	);

	// 持续回复气力：在指定持续时间内按周期累加到总量
	UFUNCTION(BlueprintCallable, Category="Cancer|Soul")
	static FActiveGameplayEffectHandle K2_ApplySoulRegen(
		AActor* Target,
		float TotalSoul,
		float DurationSeconds,
		TSubclassOf<class UCancerEffect_Soul> SoulGE = nullptr
	);


#pragma endregion

#pragma region  Debug相关
	// 判断某个 AbilityClass 是否“此刻就能激活”
	// bIsGranted: 是否已授予该能力（未授予肯定无法激活）
	// FailureTags: 可选的失败原因标签集合（可用于 UI 提示）
	UFUNCTION(BlueprintCallable, Category="Cancer|Ability")
	static bool K2_CanActivateAbilityByClass(AActor* Actor, TSubclassOf<UGameplayAbility> AbilityClass,
											 bool& bIsGranted, FGameplayTagContainer& FailureTags);

	// 判断 ASC 当前是否拥有某个 GameplayTag（包含 Loose/Owned）
	UFUNCTION(BlueprintPure, Category="Cancer|Ability")                  
	static bool K2_HasLooseGameplayTag(AActor* Actor, FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category="Cancer|Ability")
	static FGameplayTag K2_StringToTag(FString Str);

	UFUNCTION(BlueprintPure, Category="Cancer|Ability")
	static TArray<FAttributeInfo> K2_GetAttributeFromActor(AActor* Actor);
	UFUNCTION(BlueprintPure, Category="Cancer|Ability")
	static TArray<FAbilityInfo> K2_GetAbilityInfoFromActor(AActor* Actor);

#pragma endregion


#pragma region 构建伤害检测
	UFUNCTION(BlueprintCallable, Category  = "Cancer|Ability")
	static UCancerMeleeScan* CreateMeleeScanInstance(AActor* Owner, AActor* Causer, UCancerDamageType* DamageType,
											  UMeshComponent* SourceMesh, float Duration);

#pragma endregion

	//拼接Tag
	UFUNCTION(BlueprintPure)
	static FGameplayTag NormalizeToSegments(const FGameplayTag& Tag,int32 Number = 1,bool bForward = true);

	UFUNCTION(BlueprintCallable,Category="Cancer|Ability",meta=(Latent,WorldContext="WorldContextObject",ExpandEnumAsExecs="CoutDownOutput"))
	static void CountDown(const UObject* WorldContextObject,float TotalTime,float UpdateInterval,float&OutRemainingTime,
		ECoutDownInput CoutDownInput,ECoutDownOutput& CoutDownOutput,FLatentActionInfo LatentInfo);
	
	
	UFUNCTION(BlueprintCallable,Category="Cancer|Ability")
	static void ApplyDamage(AActor* TargetActor, AActor* InInstigator, UCancerDamageType* DamageType);
	
};
