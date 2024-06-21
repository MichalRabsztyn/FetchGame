#include "AsyncLoadingScreen.h"

#include "MoviePlayer.h"
#include "ISettingsModule.h"

#include "SLoadingScreenWidget.h"
#include "AsyncLoadingScreenSettings.h"

#define LOCTEXT_NAMESPACE "FAsyncLoadingScreenModule"

void FAsyncLoadingScreenModule::StartupModule()
{
	if (!IsRunningDedicatedServer())
	{
		FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([this]() {
			bGameHasStarted = true;
			});

		if (IsMoviePlayerEnabled() && FSlateApplication::IsInitialized())
		{
			GetMoviePlayer()->OnPrepareLoadingScreen().AddRaw(this, &FAsyncLoadingScreenModule::SetupLoadingScreen);
		}
	}

#if WITH_EDITOR
	RegisterSettings();
#endif
}

void FAsyncLoadingScreenModule::ShutdownModule()
{
	if (!IsRunningDedicatedServer() && IsMoviePlayerEnabled() && FSlateApplication::IsInitialized())
	{
		GetMoviePlayer()->OnPrepareLoadingScreen().RemoveAll(this);
	}

#if WITH_EDITOR
	UnregisterSettings();
#endif
}

void FAsyncLoadingScreenModule::SetupLoadingScreen()
{
	if (!bGameHasStarted)
	{
		return;
	}

	const UAsyncLoadingScreenSettings* const Settings = GetDefault<UAsyncLoadingScreenSettings>();
	if (!IsValid(Settings))
	{
		return;
	}

	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = Settings->bAutoCompleteWhenLoadingCompletes;
	LoadingScreen.MinimumLoadingScreenDisplayTime = Settings->MinimumLoadingScreenDisplayTime;
	LoadingScreen.bAllowEngineTick = Settings->bAllowEngineTick;
	LoadingScreen.WidgetLoadingScreen = SNew(SLoadingScreenWidget);

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

#if WITH_EDITOR
void FAsyncLoadingScreenModule::RegisterSettings()
{
	ISettingsModule* const SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
	if (SettingsModule == nullptr)
	{
		return;
	}

	SettingsModule->RegisterSettings(
		"Project", "Plugins", "AsyncLoadingScreen",
		LOCTEXT("AsyncLoadingScreenSettingsName", "Async Loading Screen"),
		LOCTEXT("AsyncLoadingScreenSettingsDescription", "Configure the Async Loading Screen."),
		GetMutableDefault<UAsyncLoadingScreenSettings>()
	);
}

void FAsyncLoadingScreenModule::UnregisterSettings()
{
	ISettingsModule* const SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
	if (SettingsModule == nullptr)
	{
		return;
	}

	SettingsModule->UnregisterSettings("Project", "Plugins", "AsyncLoadingScreen");
}

#endif //WITH_EDITOR

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAsyncLoadingScreenModule, AsyncLoadingScreen)