// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetPakEd.h"
#include "AssetPakEdStyle.h"
#include "AssetPakEdCommands.h"
#include "AssetPakEdWindow.h" // 包含新的窗口类
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
	/*
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
	*/
}

void FAssetPakEdModule::ShutdownModule()
{
	/*
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FAssetPakEdStyle::Shutdown();

	FAssetPakEdCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(AssetPakEdTabName);
	*/
}

TSharedRef<SDockTab> FAssetPakEdModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// 创建新的AssetPakEd窗口实例
	AssetPakEdWindowPtr = MakeShareable(new FAssetPakEdWindow());
	// 使用新的窗口的标签页生成函数
	return AssetPakEdWindowPtr->OnSpawnPluginTab(SpawnTabArgs);
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