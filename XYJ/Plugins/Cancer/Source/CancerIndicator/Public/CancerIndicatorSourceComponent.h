#pragma once
#include "CoreMinimal.h"
#include "IndicatorDescriptor.h"
#include "CancerIndicatorSourceComponent.generated.h"


UCLASS(ClassGroup=(Cancer), Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class CANCERINDICATOR_API UCancerIndicatorSourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCancerIndicatorSourceComponent();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	UIndicatorDescriptor* ConstructIndicator();

	UFUNCTION(BlueprintCallable, Category = Indicator)
	UIndicatorDescriptor* GetIndicator();

	UFUNCTION(BlueprintCallable, Category = Indicator)
	FGuid GetGuid() const { return Guid; };

	UPROPERTY()
	UIndicatorDescriptor* SourceIndicatorDescriptor = nullptr;
	UPROPERTY(BlueprintReadOnly, Category= Indicator)
	FGuid Guid = FGuid::NewGuid();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="是否可见")
	bool bVisible = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="显示UI")
	TSoftClassPtr<UUserWidget> IndicatorWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="基准组件名")
	FName IndicatorComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="组件插槽名")
	FName ComponentSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="是否钉在屏幕边缘")
	bool bClampToScreen = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="自动更新显示(距离)")
	bool bAutoUpdateVisible = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="最大显示距离",
		meta=(EditCondition="bAutoUpdateVisible"))
	float MaxDis = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="钉边时是否显示箭头")
	bool bShowClampToScreenArrow = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="组件无效时自动移除")
	bool bAutoRemoveWhenIndicatorComponentIsNull = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="投影模式")
	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="水平对齐")
	TEnumAsByte<EHorizontalAlignment> HAlignment = HAlign_Center;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="垂直对齐")
	TEnumAsByte<EVerticalAlignment> VAlignment = VAlign_Center;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="排序优先级（越大越后）")
	int32 Priority = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="包围盒锚点（0~1")
	FVector BoundingBoxAnchor = FVector(0.5, 0.5, 0.5);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="屏幕空间偏移")
	FVector2D ScreenSpaceOffset = FVector2D(0, 0);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Indicator, DisplayName="世界空间偏移")
	FVector WorldPositionOffset = FVector(0, 0, 0);
};
