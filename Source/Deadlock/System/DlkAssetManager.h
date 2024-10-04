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

	 /** 클론 코딩을 위한 테스트 함수 */
	static bool TestClone();

	/** returns the AssetManager singleton object */
	static UDlkAssetManager& Get();

	/** whether it leaves log for asset loading */
	// 해당 함수는 FScopeLogTime을 사용할지 유무를 판단하기 위해 사용한다:
	// - 굳이 클론 코딩할 필요 없지만, FScopeLogTime은 있다는 것을 알아두면 좋으니깐 진행해보자
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
	 // 여기에 UPROPERTY로 등록되어 있다 -> 이는 GC의 대상이 된다는 뜻이다!
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	/** used for a scope lock when modifying the list of loaded assets */
	// object 단위 locking에 대한 설명:
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
			// 여기서 AddLoadAsset은 메모리에 상주하기 위한 장치라고 생각하면 됨:
			// - 한번 등록되면 직접 내리지 않는한 Unload가 되지 않음 (== 캐싱)
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
