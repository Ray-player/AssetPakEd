// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetPakEdCommands.h"

#define LOCTEXT_NAMESPACE "FAssetPakEdModule"

void FAssetPakEdCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "AssetPakEd", "Bring up AssetPakEd window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
