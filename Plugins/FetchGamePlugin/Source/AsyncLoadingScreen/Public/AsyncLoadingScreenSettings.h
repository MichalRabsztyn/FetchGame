#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "AsyncLoadingScreenSettings.generated.h"


UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Async Loading Screen Settings"))
class ASYNCLOADINGSCREEN_API UAsyncLoadingScreenSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = "Loading Screen|Properties")
	FSoftObjectPath TexturePath;

	UPROPERTY(config, EditAnywhere, Category = "Loading Screen|Properties")
	float MinimumLoadingScreenDisplayTime;

	UPROPERTY(config, EditAnywhere, Category = "Loading Screen|Properties")
	bool bAutoCompleteWhenLoadingCompletes;

	UPROPERTY(config, EditAnywhere, Category = "Loading Screen|Properties")
	bool bAllowEngineTick;

public:
	UAsyncLoadingScreenSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) :
		Super(ObjectInitializer)
		, TexturePath(TEXT("/Game/UI/loading.loading"))
		, MinimumLoadingScreenDisplayTime(5.0f)
		, bAutoCompleteWhenLoadingCompletes(true)
		, bAllowEngineTick(true) {};
};
