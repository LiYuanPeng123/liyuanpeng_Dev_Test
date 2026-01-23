#include "CancerAbilityFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "CancerAbilitySetting.h"
#include "CancerAbilitySystemComponent.h"
#include "CancerNativeGamePlayTag.h"
#include "AbilitySystem/Effects/CancerEffect_Cure.h"
#include "AbilitySystem/Effects/CancerEffect_LevelSet.h"
#include "AbilitySystem/Effects/CancerEffect_IronBody.h"
#include "AbilitySystem/Effects/CancerEffect_Soul.h"
#include "Components/CancerCombatAttributeComponent.h"
#include "Components/CancerCosmeticsComponent.h"
#include "Components/CancerHeroComponent.h"
#include "Components/CancerMotionWarpingComponent.h"
#include "GameFramework/CancerDamageType.h"
#include "TimerManager.h"
#include "AbilitySystem/Action/FWCountdownAction.h"

UE_DISABLE_OPTIMIZATION

UCancerAbilitySystemComponent* UCancerAbilityFunctionLibrary::K2_GetCancerAbilitySystemComponent(AActor* Actor,
	bool& bIsValid, TSubclassOf<UCancerAbilitySystemComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerAbilitySystemComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerAbilitySystemComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

UCancerMotionWarpingComponent* UCancerAbilityFunctionLibrary::K2_GetCancerMotionWarpingComponent(AActor* Actor,
	bool& bIsValid, TSubclassOf<UCancerMotionWarpingComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerMotionWarpingComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerMotionWarpingComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

UCancerHeroComponent* UCancerAbilityFunctionLibrary::K2_GetCancerCombatManagerComponent(AActor* Actor, bool& bIsValid,
	TSubclassOf<UCancerHeroComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerCombatManagerComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerHeroComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}


UCancerCosmeticsComponent* UCancerAbilityFunctionLibrary::K2_GetCancerCosmeticsComponent(AActor* Actor, bool& bIsValid,
	TSubclassOf<UCancerCosmeticsComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerCosmeticsComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerCosmeticsComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

UCancerCombatAttributeComponent* UCancerAbilityFunctionLibrary::K2_GetCancerCombatAttributeComponent(AActor* Actor,
	bool& bIsValid, TSubclassOf<UCancerCombatAttributeComponent> OutputClass)
{
	bIsValid = false;
	if (auto ReturnValue = GetCancerCombatAttributeComponent(Actor))
	{
		if (!OutputClass)
		{
			OutputClass = UCancerCombatAttributeComponent::StaticClass();
		}
		bIsValid = ReturnValue->GetClass()->IsChildOf(OutputClass);
		return ReturnValue;
	}
	return nullptr;
}

bool UCancerAbilityFunctionLibrary::K2_CanActivateAbilityByClass(AActor* Actor,
                                                                 TSubclassOf<UGameplayAbility> AbilityClass,
                                                                 bool& bIsGranted, FGameplayTagContainer& FailureTags)
{
	bIsGranted = false;
	if (!IsValid(Actor) || !*AbilityClass)
	{
		return false;
	}

	UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponent(Actor);
	if (!ASC)
	{
		return false;
	}

	// 找到对应的 AbilitySpec（前提是该能力已经授予）
	const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
	if (!Spec || !Spec->Ability)
	{
		return false;
	}
	bIsGranted = true;

	// 使用能力自身的 CanActivateAbility（会综合冷却、费用、标签优先级等）
	FailureTags.Reset();
	const bool bCanActivate =
		Spec->Ability->CanActivateAbility(Spec->Handle, ASC->AbilityActorInfo.Get(), /*SourceTags*/nullptr,
		                                  /*TargetTags*/nullptr, /*OutFailureTags*/&FailureTags);

	return bCanActivate;
}

bool UCancerAbilityFunctionLibrary::K2_HasLooseGameplayTag(AActor* Actor, FGameplayTag Tag)
{
	if (!IsValid(Actor) || !Tag.IsValid())
	{
		return false;
	}
	UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponent(Actor);
	if (!ASC)
	{
		return false;
	}
	return ASC->HasMatchingGameplayTag(Tag);
}

FGameplayTag UCancerAbilityFunctionLibrary::K2_StringToTag(FString Str)
{
	return FGameplayTag::RequestGameplayTag(FName(*Str));
}

TArray<FAttributeInfo> UCancerAbilityFunctionLibrary::K2_GetAttributeFromActor(AActor* Actor)
{
	TArray<FAttributeInfo> AttributeArray;
	if (auto ASC = GetCancerAbilitySystemComponent(Actor))
	{
		for (UAttributeSet* Set : ASC->GetSpawnedAttributes())
		{
			if (!Set)
			{
				continue;
			}
			for (TFieldIterator<FStructProperty> It(Set->GetClass()); It; ++It)
			{
				if ((*It)->Struct == FGameplayAttributeData::StaticStruct())
				{
					FGameplayAttribute Attribute(*It);
					AttributeArray.Add(FAttributeInfo(*Attribute.AttributeName,
					                                  ASC->GetNumericAttribute(Attribute)));
				}
			}
		}
	}
	return AttributeArray;
}

TArray<FAbilityInfo> UCancerAbilityFunctionLibrary::K2_GetAbilityInfoFromActor(AActor* Actor)
{
	TArray<FAbilityInfo> AbilityArray;
	FGameplayTagContainer FailureTags;
	if (auto ASC = GetCancerAbilitySystemComponent(Actor))
	{
		for (FGameplayAbilitySpec& AbilitySpec : ASC->GetActivatableAbilities())
		{
			FAbilityInfo Info;
			Info.AbilityName = *ASC->CleanupName(GetNameSafe(AbilitySpec.Ability));

			Info.State = EGAModeState::NoActive;
			if (AbilitySpec.IsActive())
			{
				Info.State = EGAModeState::Active;
			}
			else if (ASC->IsAbilityInputBlocked(AbilitySpec.InputID))
			{
				Info.State = EGAModeState::Blocked;
				Info.BlockMessage = TEXT("输入阻挡");
			}
			else if (ASC->AreAbilityTagsBlocked(AbilitySpec.Ability->GetAssetTags()))
			{
				Info.State = EGAModeState::Blocked;
				Info.BlockMessage = TEXT("标签阻挡");
			}
			else if (AbilitySpec.Ability->CanActivateAbility(AbilitySpec.Handle, ASC->AbilityActorInfo.Get(), nullptr,
			                                                 nullptr, &FailureTags) == false)
			{
				float Cooldown = AbilitySpec.Ability->GetCooldownTimeRemaining(ASC->AbilityActorInfo.Get());
				if (Cooldown > 0.f)
				{
					Info.BlockMessage = FString::Printf(TEXT("冷却CD[%f]"), Cooldown);
					Info.State = EGAModeState::Blocked;
				}
			}
			AbilityArray.Add(Info);
		}
	}
	return AbilityArray;
}

UCancerMeleeScan* UCancerAbilityFunctionLibrary::CreateMeleeScanInstance(AActor* Owner, AActor* Causer,
	UCancerDamageType* DamageType, UMeshComponent* SourceMesh, float Duration)
{
	auto DamageParameter = DamageType->DamageParameter;
	DamageType->DamageParameter.HitInfo.SourceActor = Owner;
	TSubclassOf<UCancerMeleeScan> ScanClass = DamageParameter.TranceInfo.MeleeScanClass;
	if (!IsValid(ScanClass))
	{
		//使用默认的
		ScanClass = UCancerMeleeScan::StaticClass();
	}

	FVector ShapeDimensions = FVector::ZeroVector;
	switch (DamageParameter.TranceInfo.ScanMode)
	{
	case EMeleeScanMode::LineTrace:
		ShapeDimensions = FVector::ZeroVector;
		break;
	case EMeleeScanMode::BoxSweep:
		ShapeDimensions = DamageParameter.TranceInfo.BoxHalfExtent;
		break;
	case EMeleeScanMode::CapsuleSweep:
		ShapeDimensions = DamageParameter.TranceInfo.CapsuleExtent;
		break;
	case EMeleeScanMode::SphereSweep:
		ShapeDimensions.X = DamageParameter.TranceInfo.SphereRadius;
		break;
	default:
		break;
	}
	UCancerMeleeScan* NewMeleeScan = UCancerMeleeScan::NewInstance(
		ScanClass, Owner, Causer, SourceMesh,
		DamageParameter.TranceInfo.SocketNames,
		DamageParameter.TranceInfo.TraceSocketOffset,
		DamageParameter.TranceInfo.ScanChannels,
		DamageParameter.TranceInfo.ScanMode, ShapeDimensions,
		DamageParameter.TranceInfo.TraceRadius, Duration);

	NewMeleeScan->DamageType = DamageType;
	NewMeleeScan->bEnableDebug = DamageType->bEnableDebug;
	// Allow repeated hits on the same target according to DamageType settings
	NewMeleeScan->bAllowMultipleHitsOnSameTarget = DamageParameter.TranceInfo.bAllowMultipleHits;
	return NewMeleeScan;
}


FGameplayTag UCancerAbilityFunctionLibrary::NormalizeToSegments(const FGameplayTag& Tag, int32 Number, bool bForward)
{
	if (!Tag.IsValid()) return FGameplayTag();
	const FString TagString = Tag.ToString();
	TArray<FString> Parts;
	TagString.ParseIntoArray(Parts, TEXT("."), true);
	if (Parts.Num() < Number)
		return FGameplayTag();

	FString ResultString;
	if (bForward)
	{
		for (int32 i = 0; i < Number; ++i)
		{
			if (i != 0)
				ResultString += TEXT(".");
			ResultString += Parts[i];
		}
	}
	else
	{
		for (int32 i = Parts.Num() - Number; i < Parts.Num(); ++i)
		{
			if (!ResultString.IsEmpty())
				ResultString += TEXT(".");
			ResultString += Parts[i];
		}
	}

	return FGameplayTag::RequestGameplayTag(FName(*ResultString));
}


bool UCancerAbilityFunctionLibrary::K2_ApplyDamage_SetByCaller(
	AActor* Target,
	const UCancerDamageType* DamageObject,
	FGameplayTag OptionalDamageAssetTag,
	float DamageCoeff,
	float SoulDamageCoeff,
	FVector SoulValue3,
	TSubclassOf<UCancerEffect_Damage> DamageGE
)
{
	if (!IsValid(DamageGE))
	{
		DamageGE = UCancerAbilitySetting::Get()->DamageEffectClass;
	}
	if (!IsValid(DamageGE))
	{
		UE_LOG(LogTemp, Error, TEXT("Damage effect is invalid"));
		return false;
	}
	if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponent(Target)
	)
	{
		if (!DamageObject)return false;
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddHitResult(DamageObject->DamageParameter.HitInfo.HitResult);
		Context.AddSourceObject(DamageObject);
		AActor* SourceActor = const_cast<AActor*>(DamageObject->GetSourceActor());
		Context.AddInstigator(SourceActor, SourceActor);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageGE, 1.0f, Context);
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		if (OptionalDamageAssetTag.IsValid())
		{
			Spec->AddDynamicAssetTag(OptionalDamageAssetTag);
		}
		Spec->SetSetByCallerMagnitude(SetByCaller_DamageCoeff, DamageCoeff);
		Spec->SetSetByCallerMagnitude(SetByCaller_SoulDamageCoeff, SoulDamageCoeff);
		Spec->SetSetByCallerMagnitude(SetByCaller_DamageCoeff_X, SoulValue3.X);
		Spec->SetSetByCallerMagnitude(SetByCaller_DamageCoeff_Y, SoulValue3.Y);
		Spec->SetSetByCallerMagnitude(SetByCaller_DamageCoeff_Z, SoulValue3.Z);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec);
		return true;
	}
	return false;
}

FActiveGameplayEffectHandle UCancerAbilityFunctionLibrary::K2_ApplyDamage_SetByCaller_WithContext(
	AActor* Target,
	FGameplayTag OptionalDamageAssetTag,
	UCancerDamageType* DamageObject,
	bool& Return,
	FGameplayEffectContextHandle& OutContext,
	float DamageCoeff,
	float SoulDamageCoeff,
	FVector SoulValue3,
	TSubclassOf<UCancerEffect_Damage> DamageGE
)
{
	Return = false;
	OutContext = FGameplayEffectContextHandle();
	if (!IsValid(DamageGE)) return FActiveGameplayEffectHandle();
	if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponent(Target))
	{
		if (!DamageObject)return FActiveGameplayEffectHandle();

		AActor* SourceActor = DamageObject ? DamageObject->DamageParameter.HitInfo.SourceActor : nullptr;

		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddInstigator(SourceActor, SourceActor);
		Context.AddHitResult(DamageObject->DamageParameter.HitInfo.HitResult);
		Context.AddSourceObject(DamageObject);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageGE, 1.0f, Context);
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (OptionalDamageAssetTag.IsValid())
		{
			Spec->AddDynamicAssetTag(OptionalDamageAssetTag);
		}

		Spec->SetSetByCallerMagnitude(SetByCaller_DamageCoeff, DamageCoeff);
		Spec->SetSetByCallerMagnitude(SetByCaller_SoulDamageCoeff, SoulDamageCoeff);
		Spec->SetSetByCallerMagnitude(SetByCaller_DamageCoeff_X, SoulValue3.X);
		Spec->SetSetByCallerMagnitude(SetByCaller_DamageCoeff_Y, SoulValue3.Y);
		Spec->SetSetByCallerMagnitude(SetByCaller_DamageCoeff_Z, SoulValue3.Z);

		FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);
		OutContext = Spec->GetEffectContext();
		Return = Handle.IsValid();
		return Handle;
	}
	return FActiveGameplayEffectHandle();
}


bool UCancerAbilityFunctionLibrary::K2_ApplyIronBody(AActor* Target, int32 IronBodyLevel,
                                                     TSubclassOf<class UCancerEffect_IronBody> IronBodyGE)
{
	if (!IsValid(Target) || !IsValid(IronBodyGE))
	{
		return false;
	}
	if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponent(Target))
	{
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(IronBodyGE, 1.0f, ASC->MakeEffectContext());
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		Spec->SetSetByCallerMagnitude(SetByCaller_IronBody, IronBodyLevel);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec);
		return true;
	}
	return false;
}


FActiveGameplayEffectHandle UCancerAbilityFunctionLibrary::K2_ApplyCure_SetByCaller(AActor* Target,
	FGameplayTag OptionalCureAssetTag, bool& Return, float Cure, TSubclassOf<UCancerEffect_Cure> CureGE)
{
	Return = false;
	if (!IsValid(CureGE)) return FActiveGameplayEffectHandle();
	if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponent(Target)
	)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CureGE, 1.0f, Context);
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (OptionalCureAssetTag.IsValid())
		{
			Spec->AddDynamicAssetTag(OptionalCureAssetTag);
		}
		Spec->SetSetByCallerMagnitude(SetByCaller_Cure, Cure);
		FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);
		Return = Handle.IsValid();
		return Handle;
	}
	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle UCancerAbilityFunctionLibrary::K2_ApplyCure_SetByCaller_WithContext(
	AActor* Target,
	FGameplayTag OptionalCureAssetTag,
	bool& Return,
	FGameplayEffectContextHandle& OutContext,
	float Cure,
	TSubclassOf<UCancerEffect_Cure> CureGE
)
{
	Return = false;
	OutContext = FGameplayEffectContextHandle();
	if (!IsValid(CureGE)) return FActiveGameplayEffectHandle();
	if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponent(Target))
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CureGE, 1.0f, Context);
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (OptionalCureAssetTag.IsValid())
		{
			Spec->AddDynamicAssetTag(OptionalCureAssetTag);
		}
		Spec->SetSetByCallerMagnitude(SetByCaller_Cure, Cure);
		FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);
		OutContext = Spec->GetEffectContext();
		Return = Handle.IsValid();
		return Handle;
	}
	return FActiveGameplayEffectHandle();
}

bool UCancerAbilityFunctionLibrary::K2_ApplyGeneralGE_Instant(AActor* Source, AActor* Target, FGameplayTag PropertyTag, float Value)
{
	auto GeneralGEClass = UCancerAbilitySetting::Get()->GeneralEffectClass;
	if (!IsValid(GeneralGEClass))
	{
		UE_LOG(LogTemp, Error, TEXT("Damage effect is invalid"));
		return false;
	}
	if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponent(Target)
	)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		AActor* SourceActor = const_cast<AActor*>(Source);
		Context.AddInstigator(SourceActor, SourceActor);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GeneralGEClass, 1.0f, Context);
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		
		Spec->SetSetByCallerMagnitude(PropertyTag, Value);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec);
		return true;
	}
	return false;
}


FActiveGameplayEffectHandle UCancerAbilityFunctionLibrary::K2_ApplyLevel_SetByCaller(AActor* Target,
                                                                                     FGameplayTag OptionalLevelAssetTag, bool& Return, int32 Level,
                                                                                     float Exp, TSubclassOf<UCancerEffect_LevelSet> LevelGE)
{
	Return = false;
	if (!IsValid(LevelGE)) return FActiveGameplayEffectHandle();

	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle UCancerAbilityFunctionLibrary::K2_ApplySoulRegen(
	AActor* Target,
	float TotalSoul,
	float DurationSeconds,
	TSubclassOf<UCancerEffect_Soul> SoulGE)
{
	if (!IsValid(SoulGE)) return FActiveGameplayEffectHandle();
	if (UCancerAbilitySystemComponent* ASC = GetCancerAbilitySystemComponent(Target))
	{
		UGameplayEffect* GEDef = SoulGE->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SoulGE, 1.0f, Context);
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (!Spec)
		{
			return FActiveGameplayEffectHandle();
		}
		const float PeriodSeconds = GEDef ? FMath::Max(GEDef->Period.GetValue(), KINDA_SMALL_NUMBER) : 1.0f;
		const int32 NumTicks = FMath::Max(1, FMath::CeilToInt(DurationSeconds / PeriodSeconds));
		const float SoulPerTick = (NumTicks > 0) ? (TotalSoul / NumTicks) : TotalSoul;
		Spec->SetSetByCallerMagnitude(SetByCaller_Soul, SoulPerTick);
		FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);
		return Handle;
	}
	return FActiveGameplayEffectHandle();
}


void UCancerAbilityFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval,
                                              float& OutRemainingTime, ECoutDownInput CoutDownInput,
                                              ECoutDownOutput& CoutDownOutput,
                                              FLatentActionInfo LatentInfo)
{
	UWorld* World = GEngine
		                ? GEngine->GetWorldFromContextObject(WorldContextObject,
		                                                     EGetWorldErrorMode::LogAndReturnNull)
		                : WorldContextObject->GetWorld();
	if (!World) return;
	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
	FWCountdownAction* CountDownAction = LatentActionManager.
		FindExistingAction<FWCountdownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);
	if (CoutDownInput == ECoutDownInput::Start)
	{
		if (!CountDownAction)
		{
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget,
			                                 LatentInfo.UUID, new FWCountdownAction(TotalTime, UpdateInterval,
				                                 OutRemainingTime, CoutDownOutput, LatentInfo));
		}
	}

	if (CoutDownInput == ECoutDownInput::Cancel)
	{
		if (CountDownAction)
		{
			CountDownAction->CancelAction();
		}
	}
}

void UCancerAbilityFunctionLibrary::ApplyDamage(AActor* TargetActor, AActor* InInstigator,
	UCancerDamageType* DamageType)
{
	if (auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
	{
		FGameplayEventData Payload;
		Payload.EventTag = Tag_Combat_Event_AbilityTrigger_ReceiveHit;
		Payload.Instigator = InInstigator;
		Payload.Target = TargetActor;
		Payload.OptionalObject = DamageType;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, Payload.EventTag, Payload);
	}
}

UE_ENABLE_OPTIMIZATION
