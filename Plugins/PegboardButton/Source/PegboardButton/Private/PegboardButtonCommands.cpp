// Copyright Epic Games, Inc. All Rights Reserved.

#include "PegboardButtonCommands.h"

#define LOCTEXT_NAMESPACE "FPegboardButtonModule"

void FPegboardButtonCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "PegboardButton", "Execute PegboardButton action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
