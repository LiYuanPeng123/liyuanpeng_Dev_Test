#include "Widget/CurveGraphWidget.h"

UCurveGraphWidget::UCurveGraphWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCurveGraphWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCurveGraphWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

int32 UCurveGraphWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                     const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
                                     int32 LayerId,
                                     const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
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
	const float SpeedStep = MaxValue / NumSteps;
	for (int i = 0; i <= NumSteps; i++)
	{
		float Y = GraphSize.Y - i * YStep;
		FString Label = FString::Printf(TEXT("%f"), (float)(i * SpeedStep));
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

	if (Samples.Num() > 1)
	{
		float XStep = GraphSize.X / (float)MaxSamples;
		TArray<FVector2D> SegmentPoints;

		SegmentPoints.Add(FVector2D(0, GraphSize.Y - (Samples[0] / MaxValue) * GraphSize.Y));

		for (int i = 1; i < Samples.Num(); i++)
		{
			FVector2D P(i * XStep, GraphSize.Y - (Samples[i] / MaxValue) * GraphSize.Y);

			// 绘制前一段
			FLinearColor LineColor = NormalColor;
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

			SegmentPoints.Add(P);
		}


		// 绘制最后一段
		if (SegmentPoints.Num() > 1)
		{
			FLinearColor LineColor = NormalColor;
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

void UCurveGraphWidget::UpdateSimple(float InValue)
{
	Samples.Add(InValue);
	if (Samples.Num() > MaxSamples)
	{
		Samples.RemoveAt(0);
	}
}
