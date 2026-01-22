#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateColor.h"
#include "UObject/NameTypes.h"
#include "Templates/UniquePtr.h"

class FCancerIconStyle: public FSlateStyleSet
{
public:

	virtual const FName& GetStyleSetName() const override
	{
		return StyleName;
	}

	static FCancerIconStyle& Get()
	{
		if (!Inst.IsValid())
		{
			Inst = TUniquePtr<FCancerIconStyle>(new FCancerIconStyle);
		}
		return *(Inst.Get());
	}
	
	void Initialize();
	
	static void Shutdown()
	{
		Inst.Reset();
	}

	virtual ~FCancerIconStyle() override;

	const FVector2D Icon7x16;
	const FVector2D Icon8x4;
	const FVector2D Icon16x4;
	const FVector2D Icon8x8;
	const FVector2D Icon10x10;
	const FVector2D Icon12x12;
	const FVector2D Icon12x16;
	const FVector2D Icon14x14;
	const FVector2D Icon16x16;
	const FVector2D Icon16x20;
	const FVector2D Icon20x20;
	const FVector2D Icon22x22;
	const FVector2D Icon24x24;
	const FVector2D Icon25x25;
	const FVector2D Icon32x32;
	const FVector2D Icon40x40;
	const FVector2D Icon48x48;
	const FVector2D Icon64x64;
	const FVector2D Icon36x24;
	const FVector2D Icon128x128;
	
private:
	FCancerIconStyle();
	
	static FName StyleName;
	static TUniquePtr<FCancerIconStyle> Inst;
};
