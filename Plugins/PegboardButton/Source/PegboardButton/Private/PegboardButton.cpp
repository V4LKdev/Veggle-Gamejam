// Copyright Epic Games, Inc. All Rights Reserved.

#include "PegboardButton.h"

#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "PegboardButtonStyle.h"
#include "PegboardButtonCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName PegboardButtonTabName("PegboardButton");

#define LOCTEXT_NAMESPACE "FPegboardButtonModule"

void FPegboardButtonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FPegboardButtonStyle::Initialize();
	FPegboardButtonStyle::ReloadTextures();

	FPegboardButtonCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FPegboardButtonCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FPegboardButtonModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPegboardButtonModule::RegisterMenus));
}

void FPegboardButtonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FPegboardButtonStyle::Shutdown();

	FPegboardButtonCommands::Unregister();
}

void FPegboardButtonModule::PluginButtonClicked()
{
	const FString WidgetPath = "/Game/Pegboard/Board/Constructor/EUW_PatternDesigner.EUW_PatternDesigner";

	if (UObject* LoadedObj = StaticLoadObject(UEditorUtilityWidgetBlueprint::StaticClass(), nullptr, *WidgetPath))
	{
		if (UEditorUtilityWidgetBlueprint* WidgetBP = Cast<UEditorUtilityWidgetBlueprint>(LoadedObj))
		{
			if (UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
			{
				EditorUtilitySubsystem->SpawnAndRegisterTab(WidgetBP);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load Editor Utility Widget at path: %s"), *WidgetPath);
	}
}

void FPegboardButtonModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FPegboardButtonCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FPegboardButtonCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPegboardButtonModule, PegboardButton)