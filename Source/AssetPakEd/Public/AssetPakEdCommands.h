// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "AssetPakEdStyle.h"

class FAssetPakEdCommands : public TCommands<FAssetPakEdCommands>
{
public:

	FAssetPakEdCommands()
		: TCommands<FAssetPakEdCommands>(TEXT("AssetPakEd"), NSLOCTEXT("Contexts", "AssetPakEd", "AssetPakEd Plugin"), NAME_None, FAssetPakEdStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};