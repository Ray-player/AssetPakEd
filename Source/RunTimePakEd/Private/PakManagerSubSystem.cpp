// Fill out your copyright notice in the Description page of Project Settings.


#include "PakManagerSubSystem.h"

#include "IPlatformFilePak.h"
#include "Engine/StaticMeshActor.h"

void UPakManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UPakManagerSubSystem::Deinitialize()
{
	Super::Deinitialize();
}

void UPakManagerSubSystem::LoadPakAsset(FString PakFilePath)
{
	FString PakFileFullPath = PakFilePath;
	FPakPlatformFile* PakPlatformFile = (FPakPlatformFile*)FPlatformFileManager::Get().GetPlatformFile(FPakPlatformFile::GetTypeName());
	if (!PakPlatformFile)
	{
		UE_LOG(LogTemp, Log, TEXT("GetPlatformFile(TEXT(\"PakFile\") is NULL"));
		return;
	}
	PakFileFullPath = FPaths::ConvertRelativePathToFull(PakFileFullPath);
	TRefCountPtr<FPakFile> TmpPak = new FPakFile(PakPlatformFile, *PakFileFullPath, false);
	FString MountPoint = TmpPak->GetMountPoint();
	int32 PosContent = MountPoint.Find("../../../", ESearchCase::Type::IgnoreCase, ESearchDir::FromEnd);
	FString NewMountPoint = MountPoint.LeftChop(PosContent);

	if (FPaths::FileExists(PakFileFullPath) && FPaths::GetExtension(PakFileFullPath) == TEXT("pak"))
	{
		// 注意 ProjectSetting 中的 UseIoStore需要取消勾选，否则挂载不上
		if (bool MountRet = this->MountPakAsset(*PakFileFullPath, 0, *MountPoint))
		{
			TArray<FString> FoundFileNames;
			TmpPak->FindPrunedFilesAtPath(FoundFileNames, *MountPoint, true, false, true);
			if (FoundFileNames.Num() > 0)
			{
				for (FString& FileName : FoundFileNames)
				{
					if (FileName.EndsWith(".uasset"))
					{
						FString NewFileName = FileName;
						NewFileName.RemoveFromEnd(TEXT(".uasset"));
						int32 Pos = NewFileName.Find("/Content/");
						NewFileName = NewFileName.RightChop(Pos + 8);
						NewFileName = "/Game" + NewFileName;
						UObject* LoadedObj = StaticLoadObject(UObject::StaticClass(), NULL, *NewFileName);

						if (LoadedObj)
						{
							UE_LOG(LogTemp, Warning, TEXT("加载pak文件成功"));
							UStaticMesh* SM = Cast<UStaticMesh>(LoadedObj);
							if (SM)
							{
								AStaticMeshActor* SMA = GetWorld()->SpawnActor<AStaticMeshActor>();
								if (SMA)
								{
									SMA->SetMobility(EComponentMobility::Movable);
									SMA->GetStaticMeshComponent()->SetStaticMesh(SM);
								}
								SMA->SetActorLocation(FVector(200.f, 0.f, 100.f));
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("load pak file failed"));
						}
					}
				}
			}
		}
	}
}

void UPakManagerSubSystem::ExecMountPakAsset(FString InPakPath, int32 InPakOrder, FString InMountPoint)
{
	MountPakAsset(InPakPath, InPakOrder, InMountPoint);
}

bool UPakManagerSubSystem::MountPakAsset(const FString& PakPath, int32 PakOrder, const FString& InMountPoint)
{
	bool bMounted = false;

	FPakPlatformFile* PakFileMgr = (FPakPlatformFile*)FPlatformFileManager::Get().GetPlatformFile(FPakPlatformFile::GetTypeName());
	if (!PakFileMgr)
	{
		UE_LOG(LogPakFile, Log, TEXT("GetPlatformFile(TEXT(\"PakFile\") is NULL"));
		return false;
	}

	PakOrder = FMath::Max(0, PakOrder);

	if (FPaths::FileExists(PakPath) && FPaths::GetExtension(PakPath) == TEXT("pak"))
	{
		bool bIsEmptyMountPoint = InMountPoint.IsEmpty();
		const TCHAR* MountPoint = bIsEmptyMountPoint ? NULL : InMountPoint.GetCharArray().GetData();

#if !WITH_EDITOR
		
		if (PakFileMgr->Mount(*PakPath, PakOrder, MountPoint))
		{
			UE_LOG(LogHotPatcher, Log, TEXT("Mounted = %s, Order = %d, MountPoint = %s"), *PakPath, PakOrder, !MountPoint ? TEXT("(NULL)") : MountPoint);
			bMounted = true;
		}
		else {
			UE_LOG(LogHotPatcher, Error, TEXT("Faild to mount pak = %s"), *PakPath);
			bMounted = false;
		}

#endif
	}

	return bMounted;
}

bool UPakManagerSubSystem::UnMountPakAsset(const FString& PakPath)
{
	//bool bMounted = false;

	FPakPlatformFile* PakFileMgr = (FPakPlatformFile*)FPlatformFileManager::Get().GetPlatformFile(FPakPlatformFile::GetTypeName());
	if (!PakFileMgr)
	{
		UE_LOG(LogPakFile, Log, TEXT("GetPlatformFile(TEXT(\"PakFile\") is NULL"));
		return false;
	}

	if (!FPaths::FileExists(PakPath))
		return false;
	return PakFileMgr->Unmount(*PakPath);
}
