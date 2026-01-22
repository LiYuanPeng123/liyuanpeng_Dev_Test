
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpeedGraphWidget.generated.h"


UCLASS()
class CANCERANIMATION_API USpeedGraphWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	USpeedGraphWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
		int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	
	UFUNCTION(BlueprintCallable)
	void SetTargetCharacter(ACharacter* InCharacter);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	FVector2D GraphSize = FVector2D(400.f, 150.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	FLinearColor BackgroundColor = FLinearColor(0.f, 0.f, 0.f, 0.5f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int32 MaxSamples = 200; // 横向最大采样点

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int32 NumSteps = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int32 NumSize = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	float MaxSpeed = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	FLinearColor NormalColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	FLinearColor RootMotionColor = FLinearColor(1.f, 0.f, 0.f, 1.f);
private:
	/** 目标角色 */
	TWeakObjectPtr<ACharacter> TargetCharacter;

	/** 速度历史记录 */
	mutable TArray<float> SpeedSamples;

	/** Root Motion 历史状态 */
	mutable TArray<bool> RootMotionSamples;
	
};
