#include "Widgets/SpeedGraphWidget.h"
#include "GameFramework/Character.h"

USpeedGraphWidget::USpeedGraphWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USpeedGraphWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (!TargetCharacter.IsValid())
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			TargetCharacter = Cast<ACharacter>(PC->GetPawn());
		}
	}
}

void USpeedGraphWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!TargetCharacter.IsValid())
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			TargetCharacter = Cast<ACharacter>(PC->GetPawn());
			return;
		}
	};

	// 记录速度
	float Speed = TargetCharacter->GetVelocity().Size();
	SpeedSamples.Add(Speed);

	// 记录 Root Motion 状态
	bool bCurrentRootMotion = false;
	if (TargetCharacter->GetMesh() && TargetCharacter->GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInst = TargetCharacter->GetMesh()->GetAnimInstance();
		bCurrentRootMotion = AnimInst->GetRootMotionMontageInstance() != nullptr;
	}
	RootMotionSamples.Add(bCurrentRootMotion);

	// 保持数组长度
	if (SpeedSamples.Num() > MaxSamples)
	{
		SpeedSamples.RemoveAt(0);
		RootMotionSamples.RemoveAt(0);
	}
}

int32 USpeedGraphWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                     const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
                                     int32 LayerId,
                                     const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FVector2D DrawPos = AllottedGeometry.GetAbsolutePosition();
	const FVector2D DrawSize = GraphSize;

	// 画背景
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2f(GraphSize), FSlateLayoutTransform()),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		BackgroundColor
	);

	// 纵坐标刻度线 & 数字
	
	const float YStep = GraphSize.Y / NumSteps;
	const float SpeedStep = MaxSpeed / NumSteps;
	for (int i = 0; i <= NumSteps; i++)
	{
		float Y = GraphSize.Y - i * YStep;
		FString Label = FString::Printf(TEXT("%d"), (int32)(i * SpeedStep));
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(FVector2f(40.f, 15.f), FSlateLayoutTransform(FVector2f(5.f, Y - 8.f))),
			FText::FromString(Label),
			FCoreStyle::GetDefaultFontStyle("Regular", NumSize),
			ESlateDrawEffect::None,
			FLinearColor::White
		);
	}

	if (SpeedSamples.Num() > 1)
	{
		float XStep = GraphSize.X / (float)MaxSamples;
		TArray<FVector2D> SegmentPoints;
		bool CurrentColor = RootMotionSamples[1];

		SegmentPoints.Add(FVector2D(0, GraphSize.Y - (SpeedSamples[0] / MaxSpeed) * GraphSize.Y));

		for (int i = 1; i < SpeedSamples.Num(); i++)
		{
			FVector2D P(i * XStep, GraphSize.Y - (SpeedSamples[i] / MaxSpeed) * GraphSize.Y);

			// 判断颜色是否变化
			if (RootMotionSamples[i] != CurrentColor)
			{
				// 绘制前一段
				FLinearColor LineColor = CurrentColor ? RootMotionColor : NormalColor;
				FSlateDrawElement::MakeLines(
					OutDrawElements,
					LayerId,
					AllottedGeometry.ToPaintGeometry(),
					SegmentPoints,
					ESlateDrawEffect::None,
					LineColor,
					true,
					2.f
				);
				// 开始新段
				FVector2D LastPoint = SegmentPoints.Last(); // 先保存上一段最后一个点
				SegmentPoints.Empty();
				SegmentPoints.Add(LastPoint); // 用保存的点接续
				CurrentColor = RootMotionSamples[i];
			}

			SegmentPoints.Add(P);
		}

		// 绘制最后一段
		if (SegmentPoints.Num() > 1)
		{
			FLinearColor LineColor = CurrentColor ? RootMotionColor:NormalColor;
			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(),
				SegmentPoints,
				ESlateDrawEffect::None,
				LineColor,
				true,
				2.f
			);
		}
	}
	return LayerId + 2;
}

void USpeedGraphWidget::SetTargetCharacter(ACharacter* InCharacter)
{
	TargetCharacter = InCharacter;
}
