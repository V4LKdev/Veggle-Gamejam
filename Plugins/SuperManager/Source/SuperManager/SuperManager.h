// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSuperManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	
#pragma region ContentBrowserMenuExtension

	void InitCBMenuExtension();
	
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);
	
	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder);

	void OnDeleteUnusedAssetButtonPressed();

	void OnMikeifyButtonPressed();

	static void FixUpRedirectors();

	void SaveWorldIfDirty();

	void OnMikePressed();

	TArray<FString> FolderPathsSelected;

	TSharedPtr<SNotificationItem> StoredNotification;

	
#pragma endregion //ContentBrowserMenuExtension
};
