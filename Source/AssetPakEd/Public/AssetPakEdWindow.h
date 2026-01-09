// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// 定义资产条目结构
struct FAssetEntry
{
public:
	FAssetEntry(const FString& InAssetPath)
	{
		AssetPath = InAssetPath;
	}

	FString AssetPath;
	FString AssetType;
};

/**
 * AssetPakEd窗口类 - 用于管理和打包项目中的内容到pak文件
 */
class FAssetPakEdWindow : public TSharedFromThis<FAssetPakEdWindow>
{
public:
	virtual ~FAssetPakEdWindow();

	// 创建并返回插件标签页
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	// 资产相关数组
	TArray<TSharedPtr<FAssetEntry>> AllAssets;
	TArray<TSharedPtr<FAssetEntry>> Assets;

	// UI事件处理函数
	void OnFolderPathCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	void OnFilterTextChanged(const FText& InText);
	bool OnFilterVerifyTextChanged(const FText& InText, FText& OutText);
	void OnFilterTextCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	// 获取当前值
	FText GetCurrentFolderPath() const;

	// 按钮点击处理
	FReply HandleBrowseButtonClicked();      // 浏览文件夹按钮
	FReply HandleCookButtonClicked();        // Cook按钮
	FReply HandlePakButtonClicked();         // Pak按钮

	// 用于更新UI显示的函数
	void AddLogMessage(const FString& Message);


private:
	// 列表视图生成函数
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FAssetEntry> Asset, const TSharedRef<STableViewBase>& OwnerTable);

	// 刷新日志显示
	EActiveTimerReturnType RefreshLog(double InCurrentTime, float InDeltaTime);

	// 初始化和填充资产列表
	void PopulateAssetList(const FString& ProjectContentDirectory);
	void FilterAssetList(const FString& InFilterText);

	// UI控件引用
	TSharedPtr<SListView<TSharedPtr<FAssetEntry>>> AssetListWidget;
	TSharedPtr<class SButton> BrowseButton;
	TSharedPtr<class SButton> CookButton;
	TSharedPtr<class SButton> PakButton;
	TSharedPtr<class SEditableTextBox> FolderPathInput;
	TSharedPtr<class SEditableTextBox> FilterInput;

	// 当前状态变量
	FString CurrentFolderPath = TEXT("");

	// 进度条相关
	TSharedPtr<STextBlock> CookProgressText;
	TSharedPtr<STextBlock> PakProgressText;
};
