#include "Widget/CancerAttributeBarWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameplayMessageSubsystem.h"

UCancerAttributeBarWidget::UCancerAttributeBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCancerAttributeBarWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    // 在编辑器预览阶段也应用外观，便于实时预览图片与颜色
    ApplyAppearance();
}

void UCancerAttributeBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 运行时确保应用外观
	ApplyAppearance();

	// 解析 ASC 并绑定
	CachedASC = ResolveASC();
	BindToAttributes();
	UpdatePercent();
}

void UCancerAttributeBarWidget::NativeDestruct()
{
	UnbindFromAttributes();
	CachedASC = nullptr;
	Super::NativeDestruct();
}

void UCancerAttributeBarWidget::SetSourceActor(AActor* InActor)
{
	SourceActor = InActor;
	// 重新解析并绑定
	UnbindFromAttributes();
	CachedASC = ResolveASC();
	BindToAttributes();
	UpdatePercent();
}

void UCancerAttributeBarWidget::Refresh()
{
	UpdatePercent();
}

void UCancerAttributeBarWidget::ApplyAppearance()
{
    if (AttributeProgress)
    {
        // 应用背景与填充 Brush（包含图片与 Tint）
    	FProgressBarStyle InStyle;
    	InStyle.BackgroundImage = ProgressBackgroundBrush;
    	InStyle.FillImage = ProgressFillBrush;
    	AttributeProgress->SetWidgetStyle(InStyle);
        /*AttributeProgress->WidgetStyle.BackgroundImage = ProgressBackgroundBrush;
        AttributeProgress->WidgetStyle.FillImage = ProgressFillBrush;*/
        // 应用填充颜色（与 FillImage 的 Tint 共同作用）
        AttributeProgress->SetFillColorAndOpacity(ProgressFillColor);
    }
    // 文本颜色在 UpdatePercent 的低阈值逻辑中根据状态动态设置
}

UAbilitySystemComponent* UCancerAttributeBarWidget::ResolveASC() const
{
	if (SourceActor.IsValid())
	{
		return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SourceActor.Get());
	}

	if (bAutoBindOwnerASC)
	{
		if (APawn* Pawn = GetOwningPlayerPawn())
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn))
			{
				return ASC;
			}
		}

		if (APlayerController* PC = GetOwningPlayer())
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PC))
			{
				return ASC;
			}
		}
	}

	return nullptr;
}

void UCancerAttributeBarWidget::BindToAttributes()
{
	if (!CachedASC)
	{
		return;
	}

	if (CurrentAttribute.IsValid())
	{
		CurrentChangedHandle = CachedASC->GetGameplayAttributeValueChangeDelegate(CurrentAttribute)
			.AddUObject(this, &UCancerAttributeBarWidget::HandleAttributeChanged);
	}
	if (MaxAttribute.IsValid())
	{
		MaxChangedHandle = CachedASC->GetGameplayAttributeValueChangeDelegate(MaxAttribute)
			.AddUObject(this, &UCancerAttributeBarWidget::HandleAttributeChanged);
	}
}

void UCancerAttributeBarWidget::UnbindFromAttributes()
{
	if (!CachedASC)
	{
		CurrentChangedHandle.Reset();
		MaxChangedHandle.Reset();
		return;
	}

	if (CurrentAttribute.IsValid() && CurrentChangedHandle.IsValid())
	{
		CachedASC->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).Remove(CurrentChangedHandle);
		CurrentChangedHandle.Reset();
	}
	if (MaxAttribute.IsValid() && MaxChangedHandle.IsValid())
	{
		CachedASC->GetGameplayAttributeValueChangeDelegate(MaxAttribute).Remove(MaxChangedHandle);
		MaxChangedHandle.Reset();
	}
}

void UCancerAttributeBarWidget::HandleAttributeChanged(const FOnAttributeChangeData& Data)
{
	UpdatePercent();
}

void UCancerAttributeBarWidget::UpdatePercent()
{
	if (!AttributeProgress)
	{
		return;
	}

	float Current = 0.f;
	float Max = 0.f;

	if (CachedASC)
	{
		if (CurrentAttribute.IsValid())
		{
			Current = CachedASC->GetNumericAttribute(CurrentAttribute);
		}
		if (MaxAttribute.IsValid())
		{
			Max = CachedASC->GetNumericAttribute(MaxAttribute);
		}
	}

    float Percent = 0.f;
    if (Max > KINDA_SMALL_NUMBER)
    {
        Percent = FMath::Clamp(Current / Max, 0.f, 1.f);
    }

    AttributeProgress->SetPercent(Percent);

    // 同步文本显示：当前/最大
    if (AttributeText)
    {
        FNumberFormattingOptions Opts;
        Opts.SetMaximumFractionalDigits(0);
        const FText CurText = FText::AsNumber(Current, &Opts);
        const FText MaxText = FText::AsNumber(Max, &Opts);
        AttributeText->SetText(FText::Format(NSLOCTEXT("CancerUI", "HealthTextFmt", "{0}/{1}"), CurText, MaxText));
    }

    // 低血量高亮（可配置）
    if (bEnableLowAttributeColor)
    {
        const bool bIsLow = Percent > 0.f && Percent < LowAttributeThreshold;

        if (AttributeProgress)
        {
            AttributeProgress->SetFillColorAndOpacity(bIsLow ? LowBarColor : NormalBarColor);
        }
        if (AttributeText)
        {
            AttributeText->SetColorAndOpacity(bIsLow ? LowTextColor : NormalTextColor);
        }
    }
}
