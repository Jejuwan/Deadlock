// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkGameplayCueManager.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueSet.h"
#include "Engine/AssetManager.h"

UDlkGameplayCueManager::UDlkGameplayCueManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


UDlkGameplayCueManager* UDlkGameplayCueManager::Get()
{
	return Cast<UDlkGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

const FPrimaryAssetType UFortAssetManager_GameplayCueRefsType = TEXT("GameplayCueRefs");
const FName UFortAssetManager_GameplayCueRefsName = TEXT("GameplayCueReferences");
const FName UFortAssetManager_LoadStateClient = FName(TEXT("Client"));

void UDlkGameplayCueManager::RefreshGameplayCuePrimaryAsset()
{
	TArray<FSoftObjectPath> CuePaths;
	UGameplayCueSet* RuntimeGameplayCueSet = GetRuntimeCueSet();
	if (RuntimeGameplayCueSet)
	{
		RuntimeGameplayCueSet->GetSoftObjectPaths(CuePaths);
	}

	// ���� �߰��� ������ ��θ� CuePaths�� �߰��Ͽ�!
	FAssetBundleData BundleData;
	BundleData.AddBundleAssetsTruncated(UFortAssetManager_LoadStateClient, CuePaths);

	// PrimaryAssetId�� ������ �̸����� �����Ͽ� (const�� �����Ǿ� ����)
	FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(UFortAssetManager_GameplayCueRefsType, UFortAssetManager_GameplayCueRefsName);

	// ���� �Ŵ����� �߰� ����
	UAssetManager::Get().AddDynamicAsset(PrimaryAssetId, FSoftObjectPath(), BundleData);
}