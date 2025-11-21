// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"

#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "LevelEditorSubsystem.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{	
	InitCBMenuExtension();
}

void FSuperManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#pragma region ContentBrowserMenuExtension

void FSuperManagerModule::InitCBMenuExtension()
{
	FContentBrowserModule& CBModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBMenuExtenders = CBModule.GetAllPathViewContextMenuExtenders();
	
	CBMenuExtenders.Add(
		FContentBrowserMenuExtender_SelectedPaths::CreateRaw(
			this, &FSuperManagerModule::CustomCBMenuExtender)
			);
}

TSharedRef<FExtender> FSuperManagerModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender (new FExtender());

	if(SelectedPaths.Num()>0)
	{
		MenuExtender->AddMenuExtension(FName("Delete"),
		EExtensionHook::After,
		TSharedPtr<FUICommandList>(),
		FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddCBMenuEntry));

		FolderPathsSelected = SelectedPaths;
	}
	
	return MenuExtender;
}

void FSuperManagerModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	const FText DeleteAssetsLabel = FText::FromString("Delete Unreferenced Assets");
	const FText DeleteAssetsTooltip = FText::FromString("Safely deletes all unused assets under folder");
	
	MenuBuilder.AddMenuEntry
	(
		DeleteAssetsLabel,
		DeleteAssetsTooltip,
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetButtonPressed)
	);

	const FText MikeifyLabel = FText::FromString("Mikeify Project");
	const FText MikeifyTooltip = FText::FromString("MIKE");
	
	MenuBuilder.AddMenuEntry
	(
		MikeifyLabel,
		MikeifyTooltip,
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnMikeifyButtonPressed)
	);
}

void FSuperManagerModule::OnDeleteUnusedAssetButtonPressed()
{
	if(FolderPathsSelected.Num()>1)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("You can only do this with one folder selected"));
		return;
	}

	TArray<FString> AssetPaths = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);

	if(AssetPaths.Num()<=0)
	{
		DebugHeader::PrintLog(TEXT("No assets found in selected directory"));
		return;
	}
	if (AssetPaths.Num() > 50)
	{
		EAppReturnType::Type ConfirmedResult = DebugHeader::ShowMessageDialog(
		EAppMsgType::OkCancel,
		FString::Printf(TEXT("This Directory contains over %i elements. \n The editor might become unresponsive."),
			AssetPaths.Num()));

		if(ConfirmedResult == EAppReturnType::Cancel)
		{
			return;
		}
	}

	FixUpRedirectors();

	SaveWorldIfDirty();

	for (FString& AssetPath : AssetPaths)
	{
		UEditorAssetLibrary::SaveAsset(AssetPath, false);
	}
		
	TArray<FAssetData> UnusedAssetDataArray;

	for (const FString& AssetPath : AssetPaths)
	{
		if (AssetPath.Contains(TEXT("Collections")) ||
			AssetPath.Contains(TEXT("Developers")) ||
			AssetPath.Contains(TEXT("_ExternalActors_")) ||
			AssetPath.Contains(TEXT("_ExternalObjects_")))
		{
			continue;
		}

		if(!UEditorAssetLibrary::DoesAssetExist(AssetPath) || UEditorAssetLibrary::LoadAsset(AssetPath)->IsA<UWorld>())
		{
			continue;
		}

		TArray<FString> Referencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPath);

		if(Referencers.Num() <= 0)
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPath);
			UnusedAssetDataArray.Add(UnusedAssetData);
		}
	}

	if(UnusedAssetDataArray.Num()>0)
	{
		ObjectTools::DeleteAssets(UnusedAssetDataArray);
	}
	else
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("No unreferenced assets found"));
	}
}

void FSuperManagerModule::FixUpRedirectors()
{
	// Load module
	const IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
     
	// Create filter with asset paths
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());
     
	// Query for assets in selected paths
	TArray<FAssetData> AssetList;
	AssetRegistry.GetAssets(Filter, AssetList);
     
	if (AssetList.Num() == 0) return;
     
	// Get paths for each asset
	TArray<FString> ObjectPaths;
     
	for (const FAssetData& Asset : AssetList)
	{
		ObjectPaths.Add(Asset.GetObjectPathString());
	}
     
	// Load assets
	TArray<UObject*> Objects;

	AssetViewUtils::FLoadAssetsSettings Settings;
	Settings.bFollowRedirectors = false;
	Settings.bAllowCancel = true;

	if (const AssetViewUtils::ELoadAssetsResult Result = AssetViewUtils::LoadAssetsIfNeeded(ObjectPaths, Objects, Settings); Result != AssetViewUtils::ELoadAssetsResult::Cancelled)
	{
		// Convert objects to object redirectors
		TArray<UObjectRedirector*> Redirectors;
		for (UObject* Object : Objects)
		{
			Redirectors.Add(CastChecked<UObjectRedirector>(Object));
		}
     
		// Call fix up redirectors from asset tools
		const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
     
		AssetToolsModule.Get().FixupReferencers(Redirectors);
	}
}

void FSuperManagerModule::SaveWorldIfDirty()
{
	if (!GEditor)
	{
		return;
	}

	if (ULevelEditorSubsystem* LevelEditorSubsystem = GEditor->GetEditorSubsystem<ULevelEditorSubsystem>())
	{
		// LevelEditorSubsystem->SaveCurrentLevel();
		if (const UWorld* World = GEditor->GetEditorWorldContext().World())
		{
			if (const ULevel* CurrentLevel = World->GetCurrentLevel())
			{
				if (const UPackage* DirtyMapPackage = CurrentLevel->GetOutermost(); DirtyMapPackage->IsDirty())
				{
					LevelEditorSubsystem->SaveCurrentLevel();
				}
			}
		}
	}
}

void FSuperManagerModule::OnMikePressed()
{
	StoredNotification->SetCompletionState(SNotificationItem::CS_Success);
}

void FSuperManagerModule::OnMikeifyButtonPressed()
{
	TArray<FNotificationButtonInfo> Buttons;
	Buttons.Add(FNotificationButtonInfo(FText::FromString("MIKE"), FText(),FSimpleDelegate()));
	Buttons.Add(FNotificationButtonInfo(FText::FromString("MIKE!"), FText(), FSimpleDelegate()));

	FNotificationInfo NotificationInfo(FText::FromString("Mikeifying Project, Do Not Resist!"));
	NotificationInfo.bUseLargeFont = true;
	NotificationInfo.bUseThrobber = true;
	NotificationInfo.FadeOutDuration = 5.f;
	NotificationInfo.ExpireDuration = 10.f;
	NotificationInfo.ButtonDetails = Buttons;

	StoredNotification = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
	
	StoredNotification->SetCompletionState(SNotificationItem::CS_Pending);
	StoredNotification->Pulse(FLinearColor::Red);
}

#pragma endregion //ContentBrowserMenuExtension

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)