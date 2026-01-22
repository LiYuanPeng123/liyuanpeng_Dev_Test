
#include "CancerIconStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"


FName FCancerIconStyle::StyleName("CancerIconStyle");

TUniquePtr<FCancerIconStyle> FCancerIconStyle::Inst(nullptr);


FCancerIconStyle::~FCancerIconStyle()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*this);
}

FCancerIconStyle::FCancerIconStyle()
	: FSlateStyleSet(StyleName)
	  , Icon7x16(7.0f, 16.0f)
	  , Icon8x4(8.0f, 4.0f)
	  , Icon16x4(16.0f, 4.0f)
	  , Icon8x8(8.0f, 8.0f)
	  , Icon10x10(10.0f, 10.0f)
	  , Icon12x12(12.0f, 12.0f)
	  , Icon12x16(12.0f, 16.0f)
	  , Icon14x14(14.0f, 14.0f)
	  , Icon16x16(16.0f, 16.0f)
	  , Icon16x20(16.0f, 20.0f)
	  , Icon20x20(20.0f, 20.0f)
	  , Icon22x22(22.0f, 22.0f)
	  , Icon24x24(24.0f, 24.0f)
	  , Icon25x25(25.0f, 25.0f)
	  , Icon32x32(32.0f, 32.0f)
	  , Icon40x40(40.0f, 40.0f)
	  , Icon48x48(48.0f, 48.0f)
	  , Icon64x64(64.0f, 64.0f)
	  , Icon36x24(36.0f, 24.0f)
	  , Icon128x128(128.0f, 128.0f)

{
}

void FCancerIconStyle::Initialize()
{
	SetParentStyleName(FAppStyle::GetAppStyleSetName());
	   
	SetContentRoot(IPluginManager::Get().FindPlugin("Cancer")->GetBaseDir() / TEXT("Resources"));
	
	Set("Borderherding", new IMAGE_BRUSH("Borderherding", Icon16x16));
	Set("Notforsell", new IMAGE_BRUSH("Notforsell", Icon16x16));
	Set("Psyduck", new IMAGE_BRUSH("Psyduck", Icon16x16));
	Set("SiberianHusky", new IMAGE_BRUSH("SiberianHusky", Icon16x16));
	Set("CowCat", new IMAGE_BRUSH("CowCat", Icon16x16));
	Set("WriteCat", new IMAGE_BRUSH("WriteCat", Icon12x12));
	FSlateStyleRegistry::RegisterSlateStyle(*this);
}