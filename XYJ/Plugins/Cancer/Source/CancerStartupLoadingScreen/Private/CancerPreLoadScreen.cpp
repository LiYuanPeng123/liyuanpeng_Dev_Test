
#include "CancerPreLoadScreen.h"

#include "SCancerPreLoadingScreenWidget.h"
#include "Misc/App.h"


#define LOCTEXT_NAMESPACE "CancerPreLoadingScreen"


void FCancerPreLoadScreen::Init()
{
	if (!GIsEditor && FApp::CanEverRender())
	{
		EngineLoadingWidget = SNew(SCancerPreLoadingScreenWidget);
	}
}
#undef LOCTEXT_NAMESPACE