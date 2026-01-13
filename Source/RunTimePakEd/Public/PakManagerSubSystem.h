// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PakManagerSubSystem.generated.h"

/**
 * 
 */
UCLASS()
class RUNTIMEPAKED_API UPakManagerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category = "PakManager")
	void LoadPakAsset(FString PakFilePath);

	UFUNCTION(Exec)
	void ExecMountPakAsset(FString InPakPath, int32 InPakOrder=0, FString InMountPoint=TEXT(""));
	UFUNCTION(BlueprintCallable, Category="PakManager", meta=(AdvancedDisplay="InMountPoint"))
	bool MountPakAsset(const FString& PakPath, int32 PakOrder, const FString& InMountPoint = TEXT(""));
	UFUNCTION(BlueprintCallable, Category = "PakManager", meta = (AdvancedDisplay = "InMountPoint"))
	bool UnMountPakAsset(const FString& PakPath);
};
