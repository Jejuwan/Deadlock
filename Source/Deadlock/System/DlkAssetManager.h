// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "DlkAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	/**
	 * static methods
	 */

	 /** Ŭ�� �ڵ��� ���� �׽�Ʈ �Լ� */
	static bool TestClone();

	/** returns the AssetManager singleton object */
	static UDlkAssetManager& Get();

	/** whether it leaves log for asset loading */
	// �ش� �Լ��� FScopeLogTime�� ������� ������ �Ǵ��ϱ� ���� ����Ѵ�:
	// - ���� Ŭ�� �ڵ��� �ʿ� ������, FScopeLogTime�� �ִٴ� ���� �˾Ƶθ� �����ϱ� �����غ���
	static bool ShouldLogAssetLoads();

	/** static loading for AssetType */
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	/** returns the asset referenced by a TSoftObjectPtr; this will synchronously load the asset if it's not already loaded [Thread-Safe] */
	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	/** returns the subclass referenced by a TSoftClassPtr; this will sychronously load the asset if it's not already loaded [Thread-Safe] */
	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	/**
	 * member methods
	 */
	UDlkAssetManager();

	/** thread-safe way of adding asset to keep in memory [Thread-Safe] */
	void AddLoadedAsset(const UObject* Asset);

	/**
	 * UAssetManager interfaces
	 */
	virtual void StartInitialLoading() override;

	/**
	 * member variables
	 */
	 // ���⿡ UPROPERTY�� ��ϵǾ� �ִ� -> �̴� GC�� ����� �ȴٴ� ���̴�!
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	/** used for a scope lock when modifying the list of loaded assets */
	// object ���� locking�� ���� ����:
	// - coarse grained locking
	FCriticalSection SyncObject;
};

template <typename AssetType>
AssetType* UDlkAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}
		if (LoadedAsset && bKeepInMemory)
		{
			// added to loaded asset list.
			// ���⼭ AddLoadAsset�� �޸𸮿� �����ϱ� ���� ��ġ��� �����ϸ� ��:
			// - �ѹ� ��ϵǸ� ���� ������ �ʴ��� Unload�� ���� ���� (== ĳ��)
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}
	return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> UDlkAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}
		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}
	return LoadedSubclass;
}
