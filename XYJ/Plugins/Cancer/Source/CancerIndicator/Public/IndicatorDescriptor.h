#pragma once

#include "Components/SceneComponent.h"
#include "Types/SlateEnums.h"

#include "IndicatorDescriptor.generated.h"

#define UE_API CANCERINDICATOR_API

class SWidget;
class UIndicatorDescriptor;
class UCancerIndicatorManagerComponent;
class UUserWidget;
struct FFrame;
struct FSceneViewProjectionData;

struct FIndicatorProjection
{
	bool Project(const UIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData,
	             const FVector2f& ScreenSize, FVector& ScreenPositionWithDepth);
};

UENUM(BlueprintType)
enum class EActorCanvasProjectionMode : uint8
{
	ComponentPoint,
	ComponentBoundingBox,
	ComponentScreenBoundingBox,
	ActorBoundingBox,
	ActorScreenBoundingBox
};

/**
 * Describes and controls an active indicator.  It is highly recommended that your widget implements
 * IActorIndicatorWidget so that it can 'bind' to the associated data.
 * 描述并控制一个活动指示器。强烈建议您的控件实现 IActorIndicatorWidget 类，以便能够与相关数据进行绑定。
 */
UCLASS(MinimalAPI, BlueprintType)
class UIndicatorDescriptor : public UObject
{
	GENERATED_BODY()

public:
	UIndicatorDescriptor()
	{
	}

public:
	UFUNCTION(BlueprintCallable)
	UObject* GetDataObject() const { return DataObject; }

	UFUNCTION(BlueprintCallable)
	void SetDataObject(UObject* InDataObject) { DataObject = InDataObject; }

	UFUNCTION(BlueprintCallable)
	USceneComponent* GetSceneComponent() const { return Component; }

	UFUNCTION(BlueprintCallable)
	void SetSceneComponent(USceneComponent* InComponent) { Component = InComponent; }

	UFUNCTION(BlueprintCallable)
	FName GetComponentSocketName() const { return ComponentSocketName; }

	UFUNCTION(BlueprintCallable)
	void SetComponentSocketName(FName SocketName) { ComponentSocketName = SocketName; }

	UFUNCTION(BlueprintCallable)
	TSoftClassPtr<UUserWidget> GetIndicatorClass() const { return IndicatorWidgetClass; }

	UFUNCTION(BlueprintCallable)
	void SetIndicatorClass(TSoftClassPtr<UUserWidget> InIndicatorWidgetClass)
	{
		IndicatorWidgetClass = InIndicatorWidgetClass;
	}

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetIndicatorWidget() { return IndicatorWidget.Get(); }

public:
	// TODO Organize this better.
	TWeakObjectPtr<UUserWidget> IndicatorWidget;

public:
	UFUNCTION(BlueprintCallable)
	void SetAutoRemoveWhenIndicatorComponentIsNull(bool CanAutomaticallyRemove)
	{
		bAutoRemoveWhenIndicatorComponentIsNull = CanAutomaticallyRemove;
	}

	UFUNCTION(BlueprintCallable)
	bool GetAutoRemoveWhenIndicatorComponentIsNull() const { return bAutoRemoveWhenIndicatorComponentIsNull; }

	bool CanAutomaticallyRemove() const
	{
		return bAutoRemoveWhenIndicatorComponentIsNull && !IsValid(GetSceneComponent());
	}

public:
	// Layout Properties
	//=======================

	UFUNCTION(BlueprintCallable)
	bool GetIsVisible() const { return IsValid(GetSceneComponent()) && bVisible; }

	UFUNCTION(BlueprintCallable)
	bool GetDesiredVisibility() const { return  bVisible; }
	
	UFUNCTION(BlueprintCallable)
	void SetDesiredVisibility(bool InVisible);

	UFUNCTION(BlueprintCallable)
	EActorCanvasProjectionMode GetProjectionMode() const { return ProjectionMode; }

	UFUNCTION(BlueprintCallable)
	void SetProjectionMode(EActorCanvasProjectionMode InProjectionMode)
	{
		ProjectionMode = InProjectionMode;
	}

	// Horizontal alignment to the point in space to place the indicator at.
	UFUNCTION(BlueprintCallable)
	EHorizontalAlignment GetHAlign() const { return HAlignment; }

	UFUNCTION(BlueprintCallable)
	void SetHAlign(EHorizontalAlignment InHAlignment)
	{
		HAlignment = InHAlignment;
	}

	// Vertical alignment to the point in space to place the indicator at.
	UFUNCTION(BlueprintCallable)
	EVerticalAlignment GetVAlign() const { return VAlignment; }

	UFUNCTION(BlueprintCallable)
	void SetVAlign(EVerticalAlignment InVAlignment)
	{
		VAlignment = InVAlignment;
	}

	// Clamp the indicator to the edge of the screen?
	UFUNCTION(BlueprintCallable)
	bool GetClampToScreen() const { return bClampToScreen; }

	UFUNCTION(BlueprintCallable)
	void SetClampToScreen(bool bValue)
	{
		bClampToScreen = bValue;
	}

	// Show the arrow if clamping to the edge of the screen?
	UFUNCTION(BlueprintCallable)
	bool GetShowClampToScreenArrow() const { return bShowClampToScreenArrow; }

	UFUNCTION(BlueprintCallable)
	void SetShowClampToScreenArrow(bool bValue)
	{
		bShowClampToScreenArrow = bValue;
	}

	// The position offset for the indicator in world space.
	UFUNCTION(BlueprintCallable)
	FVector GetWorldPositionOffset() const { return WorldPositionOffset; }

	UFUNCTION(BlueprintCallable)
	void SetWorldPositionOffset(FVector Offset)
	{
		WorldPositionOffset = Offset;
	}

	// The position offset for the indicator in screen space.
	UFUNCTION(BlueprintCallable)
	FVector2D GetScreenSpaceOffset() const { return ScreenSpaceOffset; }

	UFUNCTION(BlueprintCallable)
	void SetScreenSpaceOffset(FVector2D Offset)
	{
		ScreenSpaceOffset = Offset;
	}

	UFUNCTION(BlueprintCallable)
	FVector GetBoundingBoxAnchor() const { return BoundingBoxAnchor; }

	UFUNCTION(BlueprintCallable)
	void SetBoundingBoxAnchor(FVector InBoundingBoxAnchor)
	{
		BoundingBoxAnchor = InBoundingBoxAnchor;
	}

	UFUNCTION(BlueprintCallable)
	float GetMaxDis() const { return MaxDis; }

	UFUNCTION(BlueprintCallable)
	void SetMaxDis(float InMaxDis)
	{
		MaxDis = InMaxDis;
	}

	UFUNCTION(BlueprintCallable)
	float GetDis() const { return Dis; }

	UFUNCTION(BlueprintCallable)
	void SetDis(float InDis)
	{
		Dis = InDis;
	}

	UFUNCTION(BlueprintCallable)
	bool GetAutoUpdateVisible() { return bAutoUpdateVisible; }

	UFUNCTION(BlueprintCallable)
	void SetAutoUpdateVisible(bool bValue)
	{
		bAutoUpdateVisible = bValue;
	}

	UFUNCTION(BlueprintCallable)
	bool GetInteraction() { return bInteraction; }

	UFUNCTION(BlueprintCallable)
	void SetInteraction(bool bValue);
	

public:
	// Sorting Properties
	//=======================

	// Allows sorting the indicators (after they are sorted by depth), to allow some group of indicators
	// to always be in front of others.
	UFUNCTION(BlueprintCallable)
	int32 GetPriority() const { return Priority; }

	UFUNCTION(BlueprintCallable)
	void SetPriority(int32 InPriority)
	{
		Priority = InPriority;
	}

public:
	UCancerIndicatorManagerComponent* GetIndicatorManagerComponent() { return ManagerPtr.Get(); }

	UE_API void SetIndicatorManagerComponent(UCancerIndicatorManagerComponent* InManager);

	UFUNCTION(BlueprintCallable)
	UE_API void UnregisterIndicator();

private:
	UPROPERTY()
	bool bInteraction = false;
	UPROPERTY()
	float MaxDis = 1000;
	UPROPERTY()
	float Dis = 0;
	UPROPERTY()
	bool bAutoUpdateVisible = true;
	UPROPERTY()
	bool bVisible = true;
	UPROPERTY()
	bool bClampToScreen = false;
	UPROPERTY()
	bool bShowClampToScreenArrow = false;
	UPROPERTY()
	bool bOverrideScreenPosition = false;
	UPROPERTY()
	bool bAutoRemoveWhenIndicatorComponentIsNull = false;

	UPROPERTY()
	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;
	UPROPERTY()
	TEnumAsByte<EHorizontalAlignment> HAlignment = HAlign_Center;
	UPROPERTY()
	TEnumAsByte<EVerticalAlignment> VAlignment = VAlign_Center;

	UPROPERTY()
	int32 Priority = 0; // 排序优先级（越大越后）

	UPROPERTY()
	FVector BoundingBoxAnchor = FVector(0.5, 0.5, 0.5); // 包围盒锚点（0~1）
	UPROPERTY()
	FVector2D ScreenSpaceOffset = FVector2D(0, 0); // 屏幕空间偏移
	UPROPERTY()
	FVector WorldPositionOffset = FVector(0, 0, 0); // 世界空间偏移

private:
	friend class SActorCanvas;

	UPROPERTY()
	TObjectPtr<UObject> DataObject; // 业务数据对象

	UPROPERTY()
	TObjectPtr<USceneComponent> Component; // 绑定的场景组件

	UPROPERTY()
	FName ComponentSocketName = NAME_None; // 组件插槽名（可选）

	UPROPERTY()
	TSoftClassPtr<UUserWidget> IndicatorWidgetClass; // 指示器控件类（软引用）

	UPROPERTY()
	TWeakObjectPtr<UCancerIndicatorManagerComponent> ManagerPtr; // 管理组件指针

	TWeakPtr<SWidget> Content; // 指示器内容
	TWeakPtr<SWidget> CanvasHost; // 画布宿主
};

#undef UE_API
