// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "PegboardButtonStyle.h"

class FPegboardButtonCommands : public TCommands<FPegboardButtonCommands>
{
public:

	FPegboardButtonCommands()
		: TCommands<FPegboardButtonCommands>(TEXT("PegboardButton"), NSLOCTEXT("Contexts", "PegboardButton", "PegboardButton Plugin"), NAME_None, FPegboardButtonStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
