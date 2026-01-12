// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetPakEdWindow.h"
#include "Misc/Paths.h"
#include "Misc/App.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FileHelper.h"
#include "DesktopPlatformModule.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Views/ITypedTableView.h"
#include "HAL/FileManager.h"
#include "FileHelpers.h"
#include "Widgets/SWidget.h"
#include "Async/Async.h"
#include "AssetPakEd.h"
#include "AssetPakEdStyle.h"
#include "Interfaces/IProjectManager.h"
#include "ProjectDescriptor.h"
#include "DesktopPlatformModule.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "FAssetPakEdWindow"

FAssetPakEdWindow::~FAssetPakEdWindow()
{
	// 清理资源
}

FText FAssetPakEdWindow::GetCurrentFolderPath() const
{
	return FText::FromString(CurrentFolderPath);
}


TSharedRef<SDockTab> FAssetPakEdWindow::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// 定义UI文本
	const FText SelectAssetText = LOCTEXT("SelectAssetWidgetText", "Select Assets");
	const FText FolderPathText = LOCTEXT("FolderPathWidgetText", "Project Folder Path");
	const FText HintFilterAssetsText = LOCTEXT("HintFilterAssetsWidgetText", "Filter by name");
	const FText SetFolderText = LOCTEXT("SetFolderWidgetText", "Browse");
	const FText CookButtonText = LOCTEXT("CookButtonText", "Cook");
	const FText PakButtonText = LOCTEXT("PakButtonText", "Pak");

	TSharedRef<SDockTab> PluginTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.Padding(15.0f)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SNew(SHorizontalBox)
				// 左侧：资产列表区域
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.FillWidth(0.5f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(SelectAssetText)
					]
					+ SVerticalBox::Slot()
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(10.0f)
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						[
							SAssignNew(FilterInput, SEditableTextBox)
							.HintText(HintFilterAssetsText)
							.OnTextChanged(this, &FAssetPakEdWindow::OnFilterTextChanged)
							.OnVerifyTextChanged(this, &FAssetPakEdWindow::OnFilterVerifyTextChanged)
							.OnTextCommitted(this, &FAssetPakEdWindow::OnFilterTextCommitted)
						]
					]
					+ SVerticalBox::Slot()
					.Padding(10.0f)
					.VAlign(VAlign_Fill)
					[
						SAssignNew(AssetListWidget, SListView<TSharedPtr<FAssetEntry>>)
						.ListItemsSource(&Assets)
						.SelectionMode(ESelectionMode::Type::Multi)
						.OnGenerateRow(this, &FAssetPakEdWindow::OnGenerateRowForList)
						.ScrollbarVisibility(EVisibility::Visible)
					]
				]
				// 右侧：操作面板区域
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.FillWidth(0.5f)
				[
					SNew(SVerticalBox)
					// 第一行：文件夹路径输入
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(FolderPathText)
					]
					+ SVerticalBox::Slot()
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(10.0f)
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						[
							SAssignNew(FolderPathInput, SEditableTextBox)
							.Text(this, &FAssetPakEdWindow::GetCurrentFolderPath)
							.OnTextCommitted(this, &FAssetPakEdWindow::OnFolderPathCommitted)
						]
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Right)
						.Padding(15.f, 0.f, 0.f, 0.f)
						.AutoWidth()
						[
							// 浏览资源管理器按钮
							SAssignNew(BrowseButton, SButton)
							.OnClicked(this, &FAssetPakEdWindow::HandleBrowseButtonClicked)
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							.ForegroundColor(FSlateColor::UseForeground())
							[
								SNew(STextBlock)
								.Text(SetFolderText)
							]
						]
					]
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.AutoHeight()
					.Padding(10.0f)
					[
						// 第二行：Cook按钮/pak按钮
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Left)
						.AutoWidth()
						[
							// Cook按钮
							SAssignNew(CookButton, SButton)
							.OnClicked(this, &FAssetPakEdWindow::HandleCookButtonClicked)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.ForegroundColor(FSlateColor::UseForeground())
							[
								SNew(STextBlock)
								.Text(CookButtonText)
							]
						]
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Right)
						.Padding(20.0f, 0.f, 0.f, 0.f)
						.AutoWidth()
						[
							// Pak按钮
							SAssignNew(PakButton, SButton)
							.OnClicked(this, &FAssetPakEdWindow::HandlePakButtonClicked)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.ForegroundColor(FSlateColor::UseForeground())
							[
								SNew(STextBlock)
								.Text(PakButtonText)
							]
						]
					]
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
					.FillHeight(1.f)
					.Padding(10.0f)
					[
						SAssignNew(InfoDisplayText, SMultiLineEditableText)
						.Text(FText::FromString(TEXT("AssetPakEd 运行信息:\n")))
						.IsReadOnly(true)  // 设置为只读，不允许用户编辑
						.AllowContextMenu(false)  // 禁用右键菜单
						.AutoWrapText(true)  // 自动换行
						//.VScrollBarStyle(&FAppStyle::Get().GetWidgetStyle<FScrollBarStyle>("GenericScrollBoxVertical"))
					
					]
				]
			]
		];

	// 设置标签图标
	PluginTab->SetTabIcon(FAssetPakEdStyle::Get().GetBrush("AssetPakEd.OpenPluginWindow"));

	// 初始化默认路径
	//if (FPaths::IsProjectFilePathSet())
	CurrentFolderPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

	// 填充资产列表（这里使用项目内容目录作为示例）
	//PopulateAssetList(FPaths::ProjectContentDir());

	return PluginTab;
}

void FAssetPakEdWindow::PopulateAssetList(const FString& ProjectContentDirectory)
{
	AllAssets.Empty();
	Assets.Empty();

	/* 示例：添加一些虚拟资产条目
	// 在实际实现中，这里应该扫描项目内容目录并添加真实资产
	TArray<FString> AssetPaths;

	// 扫描内容目录获取资产列表
	IFileManager& FileManager = IFileManager::Get();
	FString SearchPath = ProjectContentDirectory / TEXT("*.*");
	FileManager.FindFilesRecursive(AssetPaths, *ProjectContentDirectory, TEXT("*"), true, false);

	// 添加一些示例数据
	for (const FString& AssetPath : AssetPaths)
	{
		// 只添加某些类型的资产（如蓝图、材质等），或者全部添加
		if (AssetPath.EndsWith(TEXT(".uasset")) || AssetPath.EndsWith(TEXT(".umap")))
		{
			FString RelativePath = AssetPath;
			RelativePath.RemoveFromStart(ProjectContentDirectory);
			//RelativePath.RemoveAt(0); // 移除开头的斜杠

			TSharedPtr<FAssetEntry> Entry = MakeShareable(new FAssetEntry(RelativePath));
			AllAssets.Add(Entry);
		}
	}

	Assets = AllAssets;

	if (AssetListWidget && AssetListWidget->IsParentValid())
	{
		AssetListWidget->RebuildList();
	}*/
}

void FAssetPakEdWindow::FilterAssetList(const FString& InFilterText)
{
	Assets.Empty();

	if (InFilterText.Len() > 0)
	{
		for (const TSharedPtr<FAssetEntry>& AssetName : AllAssets)
		{
			if (AssetName->AssetPath.Contains(InFilterText))
			{
				Assets.Add(AssetName);
			}
		}
	}
	else
	{
		Assets = AllAssets;
	}

	if (AssetListWidget && AssetListWidget->IsParentValid())
	{
		AssetListWidget->RebuildList();
	}
}

TSharedRef<ITableRow> FAssetPakEdWindow::OnGenerateRowForList(TSharedPtr<FAssetEntry> Asset,
                                                              const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!Assets.IsValidIndex(0) || !Asset.IsValid() || !Asset.Get()) // 错误检查
	{
		return
				SNew(STableRow<TSharedPtr<FAssetEntry>>, OwnerTable)
				[
					SNew(SBox)
				];
	}

	return SNew(STableRow<TSharedPtr<FAssetEntry>>, OwnerTable)
		[
			SNew(SBox)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Asset.Get()->AssetPath))
				.ColorAndOpacity(FLinearColor(0.25f, 0.25f, 0.25f, 1.f))
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ShadowOffset(FIntPoint(1, 1))
				.AutoWrapText(true)
			]
		];
}

FReply FAssetPakEdWindow::HandleBrowseButtonClicked()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(BrowseButton.ToSharedRef());
		void* ParentWindowHandle = (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid())
			                           ? ParentWindow->GetNativeWindow()->GetOSWindowHandle()
			                           : nullptr;

		FString FolderName;
		const bool bFolderSelected = DesktopPlatform->OpenDirectoryDialog(
			ParentWindowHandle,
			LOCTEXT("FolderDialogTitle", "Choose a project folder").ToString(),
			CurrentFolderPath,
			FolderName
		);

		if (bFolderSelected)
		{
			UE_LOG(LogTemp, Log, TEXT("Selected Folder: %s"), *FolderName);
			CurrentFolderPath = FolderName;

			// 更新文本框
			if (FolderPathInput.IsValid())
			{
				FolderPathInput->SetText(FText::FromString(CurrentFolderPath));
			}

			// 重新填充资产列表
			PopulateAssetList(CurrentFolderPath);
		}
	}

	return FReply::Handled();
}

FReply FAssetPakEdWindow::HandleCookButtonClicked()
{
	// TODO: 实现Cook功能
	// 这里应该开始Cook流程，并更新进度条
	UE_LOG(LogTemp, Warning, TEXT("Cook button clicked - this needs implementation"));

	/* 示例：模拟Cook进度
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
	{
		// 模拟Cook过程
		for (int i = 0; i <= 100; i += 10)
		{
			CookProgressValue = i / 100.0f;

			// 在游戏线程中更新UI
			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				if (CookProgressText.IsValid())
				{
					CookProgressText->SetText(
						FText::FromString(FString::Printf(TEXT("%.0f%%"), CookProgressValue * 100)));
				}

				FString StatusMsg = FString::Printf(TEXT("Cooking... %d%%"), static_cast<int>(CookProgressValue * 100));
				AddLogMessage(StatusMsg);
			});

			FPlatformProcess::Sleep(0.1f); // 模拟工作负载
		}

		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			AddLogMessage(TEXT("Cook completed!"));
		});
	});
	*/
	return FReply::Handled();
}

FReply FAssetPakEdWindow::HandlePakButtonClicked()
{
	// TODO: 实现Pak功能
	// 这里应该开始Pak流程，并更新进度条
	UE_LOG(LogTemp, Warning, TEXT("Pak button clicked - this needs implementation"));

	/* 示例：模拟Pak进度
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
	{
		// 模拟Pak过程
		for (int i = 0; i <= 100; i += 5)
		{
			PakProgressValue = i / 100.0f;

			// 在游戏线程中更新UI
			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				if (PakProgressText.IsValid())
				{
					PakProgressText->
						SetText(FText::FromString(FString::Printf(TEXT("%.0f%%"), PakProgressValue * 100)));
				}

				FString StatusMsg = FString::Printf(TEXT("Paking... %d%%"), static_cast<int>(PakProgressValue * 100));
				AddLogMessage(StatusMsg);
			});

			FPlatformProcess::Sleep(0.15f); // 模拟工作负载
		}

		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			AddLogMessage(TEXT("Pak completed!"));
		});
	});
	*/
	return FReply::Handled();
}

void FAssetPakEdWindow::AddLogMessage(const FString& Message)
{
	// TODO: 添加日志消息显示逻辑
	UE_LOG(LogTemp, Display, TEXT("AssetPakEd Log: %s"), *Message);
	// 将消息添加到信息显示组件中
	if (InfoDisplayText.IsValid())
	{
		FText CurrentText = InfoDisplayText->GetText();
		FText NewText = FText::FromString(CurrentText.ToString() + TEXT("\n") + Message);
		InfoDisplayText->SetText(NewText);
	}
}

void FAssetPakEdWindow::AppendInfoLine(const FString& InfoText, TSharedPtr<FAssetPakEdWindow> WindowInstance)
{
	if (WindowInstance.IsValid() && WindowInstance->InfoDisplayText.IsValid())
	{
		// 在游戏线程中更新UI
		AsyncTask(ENamedThreads::GameThread, [WindowInstance, InfoText]()
		{
			FText CurrentText = WindowInstance->InfoDisplayText->GetText();
			FText NewText = FText::FromString(CurrentText.ToString() + TEXT("\n") + InfoText);
			WindowInstance->InfoDisplayText->SetText(NewText);
			
			//todo 将光标移动到文档末尾以确保滚动到底部
			//WindowInstance->InfoDisplayText->SetApplyLineHeightToBottomLine();
		});
	}
}


void FAssetPakEdWindow::OnFolderPathCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	CurrentFolderPath = InText.ToString();
	// 可以在这里保存到配置文件
}

void FAssetPakEdWindow::OnFilterTextChanged(const FText& InText)
{
	FilterAssetList(InText.ToString());
}

bool FAssetPakEdWindow::OnFilterVerifyTextChanged(const FText& InText, FText& OutText)
{
	OutText = InText;
	return true;
}

void FAssetPakEdWindow::OnFilterTextCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	FilterAssetList(InText.ToString());
}

EActiveTimerReturnType FAssetPakEdWindow::RefreshLog(double InCurrentTime, float InDeltaTime)
{
	// 如果有日志列表组件，可以刷新它
	/*
	if (LogListWidget.IsValid())
	{
		LogListWidget->RequestListRefresh();
	}
	*/

	return EActiveTimerReturnType::Continue;
}

#undef LOCTEXT_NAMESPACE
