# AssetPakEd 插件

一个用于Unreal Engine的Pak包管理和资产打包插件，提供编辑器界面和运行时Pak包挂载功能。

## 功能概述

### 运行时模块 (RunTimePakEd)

- **Pak包挂载管理**：在运行时动态挂载和卸载Pak包
- **资产加载功能**：从Pak包中加载各种类型的资产（静态网格体、通用对象等）
- **挂载状态管理**：跟踪和管理已挂载Pak包的信息
- **蓝图集成**：提供Blueprint可调用函数，便于在游戏逻辑中使用

### 编辑器模块 (AssetPakEd,功能验证中,默认未启用)

- **可视化Pak打包界面**：提供用户友好的界面来管理和打包项目资产
- **资产浏览和筛选**：可以浏览项目内容目录并筛选特定资产
- **Cook和Pak功能**：支持资产的Cook和打包操作
- **实时日志显示**：显示操作过程中的详细日志信息

## 模块结构

### AssetPakEd (编辑器模块)

- **FAssetPakEdModule**：主模块类，负责插件初始化和界面注册
- **FAssetPakEdWindow**：主窗口类，提供Pak打包的用户界面
- **FAssetEntry**：资产条目数据结构，用于管理资产信息

### RunTimePakEd (运行时模块)

- **UPakManagerSubSystem**：Pak管理器子系统，继承自UGameInstanceSubsystem
- **FMountMes**：挂载信息结构体，存储Pak包挂载状态
- **EPakResult**：枚举类型，表示Pak操作结果

## 主要特性

### 编辑器功能

- 文件夹路径浏览和设置
- 资产列表显示和筛选
- Cook和Pak操作按钮
- 实时操作日志显示

### 运行时功能

- Pak包动态挂载/卸载
- 从Pak包加载资产
- 挂载状态查询
- 支持插件Pak和普通Pak包

## 使用方法

### 编辑器使用

1. 在Unreal Editor中启用插件
2. 通过工具栏或菜单打开AssetPakEd窗口
3. 设置项目内容目录路径
4. 筛选需要打包的资产
5. 执行Cook和Pak操作

### 运行时使用（蓝图）

```cpp
// 挂载Pak包
UPakManagerSubSystem::MountPakAsset(PakPath, bIsPluginPak)

// 从Pak包加载资产
UPakManagerSubSystem::LoadAssetAsStaticMesh(AssetName)
UPakManagerSubSystem::LoadAssetAsObject(AssetName)

// 获取挂载信息
UPakManagerSubSystem::GetMountedMessage(PakName)
```

## 技术实现

### 核心机制

- 使用FPakPlatformFile进行Pak文件操作
- 通过GameInstanceSubsystem实现运行时管理
- 采用Slate框架构建编辑器界面
- 支持异步操作和状态跟踪

### 数据结构

```cpp
// 挂载信息结构
struct FMountMes {
    FString OriPakPath;      // 原始Pak路径
    FString NowMountPoint;   // 当前挂载点
    TArray<FString> PakAssets; // Pak包中的资产列表
}

// 资产条目
struct FAssetEntry {
    FString AssetPath;       // 资产路径
    FString AssetType;       // 资产类型
}
```

## 兼容性

- **引擎版本**：Unreal Engine 5.5 版本代码与蓝图
- **平台**：Windows
- **模块类型**：编辑器模块 + 运行时模块

## 目录结构

├── Source/
│ ├── AssetPakEd/ # 编辑器模块
│ │ ├── Public/ # 头文件
│ │ └── Private/ # 实现文件
│ └── RunTimePakEd/ # 运行时模块
│ ├── Public/ # 头文件
│ └── Private/ # 实现文件
├── Content/ # 插件内容资产
└── Resources/ # 图标资源

## 注意事项

插件可以加载任何路径下的Pak资源包，在控件蓝图 `BP_LoadPak` 中可以设置初始加载Pak包的路径，默认为 `Project/Saved` 文件夹下插件自带的测试pak包 `PakPluginTest_Windows_001_P.pak`；如果为编辑器环境下可以使用如图上方路径直接访问插件目录下的该Pak包
![Tips1](images/README/Tips1.png)

