#include "Data/CancerAbilitySet.h"
#include "AbilitySystemComponent.h"
#include "ActiveGameplayEffectHandle.h"


DEFINE_LOG_CATEGORY_STATIC(LogCancerAbilitySystem, Log, All)

void FCancerAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FCancerAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FCancerAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FCancerAbilitySet_GrantedHandles::TakeFromAbilitySystem(UAbilitySystemComponent* CancerASC)
{
	check(CancerASC);

	if (!CancerASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			UE_LOG(LogAbilitySystemComponent,Warning,TEXT("[%s]移除Handle:[%s]"),
				*GetNameSafe(CancerASC->GetOwner())
				,*Handle.ToString())
			CancerASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			CancerASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		CancerASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UCancerAbilitySet::UCancerAbilitySet(const FObjectInitializer& ObjectInitializer)
{
}

void UCancerAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* InASC,
                                            FCancerAbilitySet_GrantedHandles* OutGrantedHandles
                                            , UObject* SourceObject)
{
    // 向 AbilitySystem 授予本 AbilitySet 中配置的：属性集、技能与效果
    // SourceObject 通常用于标记来源（例如装备实例），以便效果归属与输入绑定等
    check(InASC);
	
    // 1) 授予属性集（Attribute Sets）
    for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
    {
        const FCancerAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogCancerAbilitySystem, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"),
			       SetIndex, *GetNameSafe(this));
			continue;
		}
    	
    	// 在 ASC Owner 上新建属性集实例，并注册到 ASC
        UAttributeSet* NewSet = NewObject<UAttributeSet>(InASC->GetOwner(), SetToGrant.AttributeSet);
    	if (DefaultStartingTable)
    	NewSet->InitFromMetaDataTable(DefaultStartingTable);
        InASC->AddAttributeSetSubobject(NewSet);
        if (OutGrantedHandles)
        {
            // 记录句柄，便于后续统一回收（TakeFromAbilitySystem）
            OutGrantedHandles->AddAttributeSet(NewSet);
        }
    }

    // 2) 根据 GrantSource 决定从“数据表”或“数组”读取授予内容
    TArray<FCancerAbilitySet_GameplayAbility*> GrantedGameplayAbilities;
    TArray<FCancerAbilitySet_GameplayEffect*> GrantedGameplayEffects;

    if (GrantAbilitySource == ECancerGrantSource::DataTables)
    {
	    // 从 DataTable 解析出行指针集合
	    GrantedGameplayAbilities = GetInfo<FCancerAbilitySet_GameplayAbility>(AbilityDataTables);
    }
    else // ECancerAbilityGrantSource::Arrays
    {
	    // 将数组元素的地址收集为指针列表，复用统一授予流程
	    for (FCancerAbilitySet_GameplayAbility& Elem : GameplayAbilitys)
	    {
		    GrantedGameplayAbilities.Add(&Elem);
	    }
    }

    if (GrantEffectSource == ECancerGrantSource::DataTables)
    {
	    GrantedGameplayEffects = GetInfo<FCancerAbilitySet_GameplayEffect>(EffectDataTables);
    }
    else
    {
	    for (FCancerAbilitySet_GameplayEffect& Elem : GameplayEffects)
	    {
		    GrantedGameplayEffects.Add(&Elem);
	    }
    }

    // 3) 授予技能（Gameplay Abilities）
    for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
    {
        const FCancerAbilitySet_GameplayAbility* AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

        if (!IsValid(AbilityToGrant->Ability))
        {
            UE_LOG(LogCancerAbilitySystem, Error,
                   TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex,
                   *GetNameSafe(this));
            continue;
        }

        // 获取技能 CDO 并构造授予的 Spec（等级、来源、输入标签）
        UGameplayAbility* AbilityCDO = AbilityToGrant->Ability->GetDefaultObject<UGameplayAbility>();

        FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant->AbilityLevel);
        AbilitySpec.SourceObject = SourceObject;
        //AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant->InputTag);

        const FGameplayAbilitySpecHandle AbilitySpecHandle = InASC->GiveAbility(AbilitySpec);

    	UE_LOG(LogCancerAbilitySystem, Warning,
				  TEXT("授予:[%s]index:[%d]技能:[%s]"), *GetNameSafe(InASC->GetOwner()),
				  AbilityIndex,
				  *AbilityCDO->GetAssetTags().ToString());
        if (OutGrantedHandles)
        {
	        // 保存技能句柄，便于卸载时统一清理
	        OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
        }
    }

    // 4) 授予效果（Gameplay Effects）
    for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
    {
        const FCancerAbilitySet_GameplayEffect* EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant->GameplayEffect))
		{
			UE_LOG(LogCancerAbilitySystem, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"),
			       EffectIndex, *GetNameSafe(this));
			continue;
		}

		        // 以指定等级将效果应用到自身
        const UGameplayEffect* GameplayEffect = EffectToGrant->GameplayEffect->GetDefaultObject<UGameplayEffect>();
        const FActiveGameplayEffectHandle GameplayEffectHandle = InASC->ApplyGameplayEffectToSelf(
            GameplayEffect, EffectToGrant->EffectLevel, InASC->MakeEffectContext());

        if (OutGrantedHandles)
        {
            // 保存效果句柄，便于卸载时统一清理
            OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
        }
    }
}
