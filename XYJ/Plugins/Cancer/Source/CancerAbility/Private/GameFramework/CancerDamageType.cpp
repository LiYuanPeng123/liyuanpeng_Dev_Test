
#include "GameFramework/CancerDamageType.h"


UCancerDamageType::UCancerDamageType()
{
	DamageParameter.TranceInfo.MeleeScanClass = UCancerMeleeScan::StaticClass();
}

const AActor* UCancerDamageType::GetSourceActor() const
{
    return DamageParameter.HitInfo.SourceActor;
}

void UCancerDamageType::SetSourceActor(AActor* InSourceActor)
{
	DamageParameter.HitInfo.SourceActor = InSourceActor;
}

ECancerHitType UCancerDamageType::GetHitType() const
{
	return DamageParameter.HitInfo.HitType;
}

const FCancerHitInfo& UCancerDamageType::GetHitInfo() const
{
	return DamageParameter.HitInfo;
}

const FCancerComboInfo& UCancerDamageType::GetComboInfo()
{
	return DamageParameter.ComboInfo;
}

const FCancerTranceInfo& UCancerDamageType::GetTraceInfo() const
{
	return DamageParameter.TranceInfo;
}

void UCancerDamageType::SetTraceInfo(const FCancerTranceInfo& InTraceInfo)
{
	DamageParameter.TranceInfo = InTraceInfo;
}

void UCancerDamageType::SetHitResult(const FHitResult& InHit)
{
	DamageParameter.HitInfo.HitResult = InHit;
}

const FCancerHitEffectInfo& UCancerDamageType::GetHitEffectInfo() const
{
	return DamageParameter.HitEffectInfo;
}

void UCancerDamageType::SetHitEffectInfo(const FCancerHitEffectInfo& InHitEffect)
{
	DamageParameter.HitEffectInfo = InHitEffect;
}

FHitResult& UCancerDamageType::GetHitResult()
{
	return DamageParameter.HitInfo.HitResult;
}

void UCancerDamageType::SetCancerHitInfo(const FCancerHitInfo& HitInfo)
{
	DamageParameter.HitInfo = HitInfo;
}

