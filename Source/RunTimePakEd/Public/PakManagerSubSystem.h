// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PakManagerSubSystem.generated.h"

class FPakPlatformFile;

UENUM(BlueprintType)
enum class EPakResult : uint8
{
	TrueOut,
	FalseOut
};

USTRUCT(BlueprintType)
struct FMountMes
{
	GENERATED_BODY()
public:
	bool IsEmpty()
	{
		return OriPakPath.IsEmpty() && NowMountPoint.IsEmpty() && PakAssets.IsEmpty();
	}
	bool HasAsset(const FString& AssetName)
	{
		return PakAssets.Contains(AssetName);
	}
	FMountMes()
	{
		OriPakPath = TEXT("");
		NowMountPoint = TEXT("");
		PakAssets.Empty();
	}
	FMountMes(FString InOriPakPath, FString InNowMountPoint, const TArray<FString>& InPakAssets)
	{
		OriPakPath = InOriPakPath;
		NowMountPoint = InNowMountPoint;
		PakAssets = InPakAssets;
	}
	UPROPERTY(BlueprintReadWrite, Category = "MountMessage")
	FString OriPakPath;
	UPROPERTY(BlueprintReadWrite, Category = "MountMessage")
	FString NowMountPoint;
	UPROPERTY(BlueprintReadWrite, Category = "MountMessage")
	TArray<FString> PakAssets;
};
/**
 * 
 */
UCLASS()
class RUNTIMEPAKED_API UPakManagerSubSystem :public UGameInstanceSubsystem //public UEngineSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	FPakPlatformFile *GetPakPlatform();
	
	UFUNCTION(BlueprintPure, Category = "PakManager")
	FMountMes GetMountedMessage(const FString& PakName);
	UFUNCTION(BlueprintPure, Category = "PakManager")
	TArray<FString> GetAllMountAssets();
	
	UFUNCTION(BlueprintCallable, Category="PakManager",meta = (ExpandEnumAsExecs = "ReturnValue"))
	EPakResult MountPakAsset(const FString& InPakPath,bool bIsPluginPak=false);
	UFUNCTION(BlueprintCallable, Category = "PakManager")
	bool UnMountPakAsset(const FString& PakPath);

	//从Pak文件中加载资产
	UFUNCTION(BlueprintPure, Category="PakManager")
	UObject* LoadAssetAsObject(const FString &AssetName);
	
	UFUNCTION(BlueprintPure, Category="PakManager")
	UStaticMesh* LoadAssetAsStaticMesh(const FString &AssetName);

	
	FMountMes* GetMountedMesFromAssetName(const FString& AssetName);
	template<class T>
	T *LoadUObjectFromPak(const FString &Filename)
	{
		//const FString Name = T::StaticClass()->GetName() + TEXT("'") + Filename + TEXT(".") + FPackageName::GetShortName(Filename) + TEXT("'");
		return Cast<T>(StaticLoadObject(T::StaticClass(), nullptr, *Filename));
	}
private:
	TSharedPtr<FPakPlatformFile> PakPlatformFile;
	TArray<FMountMes> MountMessages;
	FString ConvertPakFile(const FString& InFileName, bool bIsPluginPak);
	void RegisterLevel(const FString& LevelName) const;
#if WITH_EDITOR
	IPlatformFile* OriginalPlatformFile = nullptr;
#endif
	
};
