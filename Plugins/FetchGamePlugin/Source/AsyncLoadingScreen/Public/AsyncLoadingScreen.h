#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAsyncLoadingScreenModule : public IModuleInterface
{
private:
	bool bGameHasStarted = false;

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterSettings();
	void UnregisterSettings();

	void SetupLoadingScreen();
};