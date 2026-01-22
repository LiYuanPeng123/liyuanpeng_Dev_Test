#include "Combo/ComboInput.h"
#include "Engine/World.h"
#include "Combo/ComboInputConfig.h"
#include "Combo/ComboInputMapping.h"
#include "GameFramework/PlayerController.h"

UComboInputComponent::UComboInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UComboInputComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UComboInputComponent::BufferPressTags(const FGameplayTagContainer& Tags)
{
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	FGameplayTagContainer Logical;
	for (const FGameplayTag& T : Tags)
	{
		FGameplayTag Mapped = Mapping ? Mapping->MapPhysicalToLogical(T) : T;
		Logical.AddTag(Mapped);
		LastPressTime.Add(Mapped, Now);
	}
	ResolveAndBuffer(Logical);
}

void UComboInputComponent::BufferReleaseTags(const FGameplayTagContainer& Tags)
{
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	for (const FGameplayTag& T : Tags)
	{
		FGameplayTag Mapped = Mapping ? Mapping->MapPhysicalToLogical(T) : T;
		LastReleaseTime.Add(Mapped, Now);
	}
}

TArray<FComboResolvedInput> UComboInputComponent::GetBufferedResolvedInputs() const
{
	return ResolvedInputs;
}

void UComboInputComponent::ClearBuffer()
{
	RawInputs.Reset();
	ResolvedInputs.Reset();
}

void UComboInputComponent::FeedAxis(const FGameplayTag& AxisTag, float Value)
{
	const float Clamped = FMath::Clamp(Value, -1.f, 1.f);
	AxisValues.Add(AxisTag, Clamped);
}

void UComboInputComponent::PurgeOld()
{
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	const float BufferWindow = Config ? Config->BufferWindowSeconds : 0.25f;
	for (int32 i = ResolvedInputs.Num() - 1; i >= 0; --i)
	{
		if (Now - ResolvedInputs[i].Timestamp > BufferWindow)
		{
			ResolvedInputs.RemoveAt(i);
		}
	}
	const int32 MaxEvents = Config ? Config->MaxBufferEvents : 16;
	while (ResolvedInputs.Num() > MaxEvents)
	{
		ResolvedInputs.RemoveAt(0);
	}
}

FGameplayTagContainer UComboInputComponent::BuildDirectionalTags() const
{
	FGameplayTagContainer Out;
	const float Deadzone = Config ? Config->AxisDeadzone : 0.15f;
	const float Cardinal = Config ? Config->AxisCardinalThreshold : 0.65f;
	const float X = AxisValues.FindRef(FGameplayTag::RequestGameplayTag(FName(TEXT("Axis.MoveX")), false));
	const float Y = AxisValues.FindRef(FGameplayTag::RequestGameplayTag(FName(TEXT("Axis.MoveY")), false));
	if (FMath::Abs(X) <= Deadzone && FMath::Abs(Y) <= Deadzone)
	{
		return Out;
	}
	if (!Config || Config->DirectionReference == EComboDirectionReference::Actor)
	{
		if (Y > Cardinal) Out.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Forward")), false));
		else if (Y < -Cardinal) Out.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Back")), false));
		if (X > Cardinal) Out.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Right")), false));
		else if (X < -Cardinal) Out.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Left")), false));
	}
	else
	{
		FRotator RefRot(0.f, 0.f, 0.f);
		if (APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
		{
			const FRotator CamRot = PC->GetControlRotation();
			RefRot = FRotator(0.f, CamRot.Yaw, 0.f);
		}
		const FVector Forward = FRotationMatrix(RefRot).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(RefRot).GetUnitAxis(EAxis::Y);
		const FVector Desired = Forward * Y + Right * X;
		const float ForwardDot = FVector::DotProduct(Desired.GetSafeNormal(), Forward);
		const float RightDot = FVector::DotProduct(Desired.GetSafeNormal(), Right);
		if (ForwardDot > Cardinal) Out.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Forward")), false));
		else if (ForwardDot < -Cardinal) Out.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Back")), false));
		if (RightDot > Cardinal) Out.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Right")), false));
		else if (RightDot < -Cardinal) Out.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Dir.Left")), false));
	}
	return Out;
}

void UComboInputComponent::ResolveAndBuffer(const FGameplayTagContainer& LogicalPress)
{
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	FGameplayTagContainer Direction = BuildDirectionalTags();
	FGameplayTagContainer Total = LogicalPress;
	Total.AppendTags(Direction);

	TArray<FGameplayTag> PressedKeys;
	Total.GetGameplayTagArray(PressedKeys);

	FGameplayTag Chord = Mapping ? Mapping->TryResolveChord(PressedKeys) : FGameplayTag();
	if (Chord.IsValid())
	{
		Total.AddTag(Chord);
	}

	const float DoubleGap = Config ? Config->DoubleTapMaxGapSeconds : 0.25f;
	for (const FGameplayTag& T : PressedKeys)
	{
		const float* PrevPress = LastPressTime.Find(T);
		if (PrevPress && (Now - *PrevPress) <= DoubleGap)
		{
			Total.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tap.Double")), false));
			break;
		}
	}

	const float HoldThreshold = Config ? Config->HoldThresholdSeconds : 0.30f;
	for (const FGameplayTag& T : PressedKeys)
	{
		const float* PressTime = LastPressTime.Find(T);
		if (PressTime && (Now - *PressTime) >= HoldThreshold)
		{
			Total.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Tap.Hold")), false));
			break;
		}
	}

	FComboResolvedInput Resolved;
	Resolved.Tags = Total;
	Resolved.Timestamp = Now;
	ResolvedInputs.Add(Resolved);
	OnInputResolved.Broadcast(Resolved);
	PurgeOld();
}
