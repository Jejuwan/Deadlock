// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkAssetManager.h"
#include "Deadlock/DlkLogChannels.h"
#include "Deadlock/DlkGameplayTags.h"

PRAGMA_DISABLE_OPTIMIZATION
bool UDlkAssetManager::TestClone()
{
	// naive functionality test
	static bool bResult = false;
	{
		bResult = ShouldLogAssetLoads();
	}
	return true;
}
PRAGMA_ENABLE_OPTIMIZATION

UDlkAssetManager& UDlkAssetManager::Get()
{
	check(GEngine);

	// �츮�� AssetManager�� UEngine�� GEngine�� AssetManager�� Class�� �������̵��߱� ������, �̹� GEngine�� Asset Manager�� ����
	if (UDlkAssetManager* Singleton = Cast<UDlkAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogDlk, Fatal, TEXT("invalid AssetManagerClassName in DefaultEngine.ini(project settings); it must be DlkAssetManager"));

	// ���� UE_LOG�� Fatal�� Verbose�� ���� Crash�� ����, ������� ���������� ������, Code Completness�� ���� ���̷� ���� ����
	return *NewObject<UDlkAssetManager>();
}

bool UDlkAssetManager::ShouldLogAssetLoads()
{
	// ���⼭ �������� �˾Ƶθ� ���� �� ���� Unreal �ڵ� �������ִ�:
// 1. FParse::Param�� FCommandLine::Get()�� ����, �����е��� ���ϴ� �Ӽ����� CommandLine���� ������ �� �ִ�
//    - �ѹ� UDlkAssetManager::Get()�� ���� �ǽ��غ���
// 2. �Լ� �ȿ� static ���� �����̴�:
//    - �𸮾󿡼� ���� ���Ե� ��������, ���� �۷ι� ������ �����ϰ� ���� ���, �����п��� �ΰ��� �ɼ��� �ִ�:
//      1. .h ���Ͽ� extern�� �����Ͽ� ��������� ���Խ�Ű�ų�?
//      2. static �Լ��� �����Ͽ�, �ش� �Լ� �ȿ� static ������ �����Ͽ�, ��ȯ�ϴ� ��� (��ġ global ������ Ȱ���� �� ����)
//         - ��ǥ���� ���÷� �ռ� Singleton ������ �ǰڴ�

// �ǽ�:
// - UDlkAssetManager::TestClone ����
// - bLogAssetLoads�� �޸� �ּҸ� �ľ�
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

UObject* UDlkAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{

	// �ش� �Լ��� ���� ������ 'synchronous load asset�� ���ʿ��ϰ� �ϴ� ���� �ִ��� üũ�ϱ� ����!'
	if (AssetPath.IsValid())
	{
		// ��� FScopeLogTime�� Ȯ���غ���:
		TUniquePtr<FScopeLogTime> LogTimePtr;
		if (ShouldLogAssetLoads())
		{
			// �ܼ��� �α��ϸ鼭, �ʴ����� �α� ����
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronously loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		// ���⼭ �ΰ��� �б�:
		// 1. AssetManager�� ������, AssetManager�� StreamableManager�� ���� ���� �ε�
		// 2. �ƴϸ�, FSoftObjectPath�� ���� �ٷ� ���� �ε�
		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}

		// if asset manager is not ready, use LoadObject()
		// - �� ����, StaticLoadObject�� ���δ�: 
		// - �����, �׻� StaticLoadObject�ϱ� ���� StaticFindObject�� ���� Ȯ���ϰ� �����ϸ� ��¥ �ε���
		return AssetPath.TryLoad();
	}

	return nullptr;
}

/**
 * member methods
 */
UDlkAssetManager::UDlkAssetManager()
// �׻� Constructor�� ����� �ʵ��� ����!
	: UAssetManager()
{
}

void UDlkAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		// for thread-safety, use lock
		FScopeLock Lock(&SyncObject);
		LoadedAssets.Add(Asset);
	}
}

/**
 * UAssetManager's interfaces
 */
void UDlkAssetManager::StartInitialLoading()
{
	// �������̵��� ���, Super�� ȣ���� �� ����� ����
	Super::StartInitialLoading();

	// DlkGameplayTags �ʱ�ȭ
	// - Lyra���� �������� �ѹ� ���� ����:
	//   - STARTUP_JOB() ��ũ�θ� Ȱ���ϰ� ������, ���� �츮�� �ε��� ���� �����Ƿ� �����ϰ� ����
	FDlkGameplayTags::InitializeNativeTags();
}
