
#pragma once

#include "CoreMinimal.h"


#pragma once

#include "PreLoadScreenBase.h"

class SWidget;

class FCancerPreLoadScreen : public FPreLoadScreenBase
{
public:
	
	/*** IPreLoadScreen Implementation ***/
	virtual void Init() override;
	virtual EPreLoadScreenTypes GetPreLoadScreenType() const override { return EPreLoadScreenTypes::EngineLoadingScreen; }
	virtual TSharedPtr<SWidget> GetWidget() override { return EngineLoadingWidget; }
private:

	TSharedPtr<SWidget> EngineLoadingWidget;
};


