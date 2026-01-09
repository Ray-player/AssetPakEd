// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetPakEd.h"
#include "AssetPakEdStyle.h"
#include "AssetPakEdCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName AssetPakEdTabName("AssetPakEd");

#define LOCTEXT_NAMESPACE "FAssetPakEdModule"

void FAssetPakEdModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FAssetPakEdStyle::Initialize();
	FAssetPakEdStyle::ReloadTextures();

	FAssetPakEdCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FAssetPakEdCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FAssetPakEdModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAssetPakEdModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(AssetPakEdTabName, FOnSpawnTab::CreateRaw(this, &FAssetPakEdModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FAssetPakEdTabTitle", "AssetPakEd"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FAssetPakEdModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FAssetPakEdStyle::Shutdown();

	FAssetPakEdCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(AssetPakEdTabName);
}

TSharedRef<SDockTab> FAssetPakEdModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FAssetPakEdModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("AssetPakEd.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FAssetPakEdModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(AssetPakEdTabName);
}

void FAssetPakEdModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FAssetPakEdCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FAssetPakEdCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAssetPakEdModule, AssetPakEd)