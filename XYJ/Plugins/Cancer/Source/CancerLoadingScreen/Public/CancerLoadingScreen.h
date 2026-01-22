
#pragma once

#include "Modules/ModuleManager.h"

class FCancerLoadingScreenModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	FDelegateHandle DelegateHandle;
	void PreSetupLoadingScreen();

	
	void RegisterMovieStreamer(TSharedPtr<IMovieStreamer> MovieStreamer);
};
