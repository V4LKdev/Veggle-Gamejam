// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleGameInstance.h"

#include "GameFramework/GameUserSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleGameInstance)

UVeggleGameInstance::UVeggleGameInstance()
{
}

void UVeggleGameInstance::Init()
{
	Super::Init();

	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		UserSettings->SetScreenResolution(FIntPoint(1440, 1080));
		UserSettings->SetFullscreenMode(EWindowMode::Windowed);
		UserSettings->ApplyResolutionSettings(false);
		UserSettings->ApplySettings(false);
	}
}
