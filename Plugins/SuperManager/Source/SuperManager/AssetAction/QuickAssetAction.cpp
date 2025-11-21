// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickAssetAction.h"

#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "SuperManager/DebugHeader.h"

void UQuickAssetAction::TestFunc()
{
	DebugHeader::Print(TEXT("TestFunc Called"), FColor::Cyan);
	DebugHeader::PrintLog(TEXT("TestFunc Called"));
}

void UQuickAssetAction::DuplicateAssets(const int32 NumDupes)
{
	if (NumDupes <= 0)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("Please enter a value greater than 0"));
		return;
	}

	TArray<FAssetData> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;

	for (const FAssetData& SelectedData : SelectedAssets)
	{
		for (int32 i = 0; i < NumDupes; i++)
		{
			const FString SourcePath = SelectedData.GetObjectPathString();
			const FString DuplicatedAssetName = SelectedData.AssetName.ToString() + TEXT("_") + FString::FromInt(i + 1);
			const FString DestinationAssetPath = FPaths::Combine(SelectedData.PackagePath.ToString(),
			                                                     DuplicatedAssetName);

			if (UEditorAssetLibrary::DuplicateAsset(SourcePath, DestinationAssetPath))
			{
				UEditorAssetLibrary::SaveAsset(DestinationAssetPath, false);
				++Counter;
			}
		}
	}

	if (Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(FString::Printf(TEXT("Successfully duplicated %i assets"), Counter));
	}
}

void UQuickAssetAction::AddPrefixes()
{
	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter = 0;

	for (UObject* SelectedObject : SelectedObjects)
	{
		if (!SelectedObject)
		{
			continue;
		}

			const FString* FoundPrefix = PrefixMap.Find(SelectedObject->GetClass());
			if (!FoundPrefix || FoundPrefix->IsEmpty())
			{
				DebugHeader::Print((TEXT("Failed to find prefix for class %s"), SelectedObject->GetClass()->GetName()), FColor::Red);
				continue;
			}

			FString OldName = SelectedObject->GetName();

			if (OldName.StartsWith(*FoundPrefix))
			{
				continue;
			}

			if (SelectedObject->IsA<UMaterialInstanceConstant>())
			{
				OldName.RemoveFromStart(TEXT("M_"));
				OldName.RemoveFromEnd(TEXT("_Inst"));
			}

			const FString NewName = *FoundPrefix + OldName;

			UEditorUtilityLibrary::RenameAsset(SelectedObject, NewName);
			Counter++;
		}

		DebugHeader::ShowNotifyInfo(FString::Printf(TEXT("Renamed %i Assets"), Counter));
	}

void UQuickAssetAction::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetData;

	FixUpRedirectors();

	for (const FAssetData& SelectedData : SelectedAssetsData)
	{
		if(TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedData.PackageName.ToString()); AssetReferencers.Num()==0)
		{
			UnusedAssetData.Add(SelectedData);
		}
	}

	if(UnusedAssetData.Num()==0)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("No unused assets found"), false);
		return;
	}

	const int32 NumDeletedAssets = ObjectTools::DeleteAssets(UnusedAssetData);

	if(NumDeletedAssets == 0)
	{
		return;
	}

	DebugHeader::ShowNotifyInfo(FString::Printf(TEXT("Successfully deleted %i unused assets"), NumDeletedAssets));
}

void UQuickAssetAction::FixUpRedirectors()
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
		const FAssetToolsModule& AssetToolsModule =
			FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
     
		AssetToolsModule.Get().FixupReferencers(Redirectors);
	}
}

void UQuickAssetAction::MikeifyAssets()
{
	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter = 0;

	for (UObject* SelectedObject : SelectedObjects)
	{
		if (!SelectedObject)
		{
			continue;
		}

		const FString NewName = "Mike_" + FString::FromInt(Counter);

		UEditorUtilityLibrary::RenameAsset(SelectedObject, NewName);
		Counter++;
	}

	DebugHeader::ShowNotifyInfo(FString::Printf(TEXT("Mikeifyed %i Assets"), Counter));
}
