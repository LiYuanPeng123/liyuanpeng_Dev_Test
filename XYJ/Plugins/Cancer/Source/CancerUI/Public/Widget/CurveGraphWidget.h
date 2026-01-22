
#pragma once

#include "CoreMinimal.h"
#include "CancerUserWidget.h"
#include "CurveGraphWidget.generated.h"

/**
 * 
 */
UCLASS()
class CANCERUI_API UCurveGraphWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UCurveGraphWidget(const FObjectInitializer& ObjectInitializer);


	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	                          const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
	                          int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;


	UFUNCTION(BlueprintCallable)
	void UpdateSimple(float InValue);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	FVector2D GraphSize = FVector2D(400.f, 150.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	FLinearColor BackgroundColor = FLinearColor(0.f, 0.f, 0.f, 0.5f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int32 MaxSamples = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int32 NumSteps = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int32 NumSize = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	float MaxValue = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	FLinearColor NormalColor = FLinearColor(1.f, 1.f, 1.f, 1.f);

private:
	/** 历史记录 */
	mutable TArray<float> Samples;
};
