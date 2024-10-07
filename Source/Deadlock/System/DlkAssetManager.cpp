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

	// 우리는 AssetManager를 UEngine의 GEngine의 AssetManager의 Class를 오버라이드했기 때문에, 이미 GEngine에 Asset Manager가 있음
	if (UDlkAssetManager* Singleton = Cast<UDlkAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogDlk, Fatal, TEXT("invalid AssetManagerClassName in DefaultEngine.ini(project settings); it must be DlkAssetManager"));

	// 위의 UE_LOG의 Fatal의 Verbose가 물론 Crash를 통해, 여기까지 도달하지는 않지만, Code Completness를 위해 더미로 리턴 진행
	return *NewObject<UDlkAssetManager>();
}

bool UDlkAssetManager::ShouldLogAssetLoads()
{
	// 여기서 여러분이 알아두면 좋은 두 가지 Unreal 코딩 패턴이있다:
// 1. FParse::Param과 FCommandLine::Get()을 통해, 여러분들이 원하는 속성값은 CommandLine으로 가져올 수 있다
//    - 한번 UDlkAssetManager::Get()을 통해 실습해보자
// 2. 함수 안에 static 변수 선언이다:
//    - 언리얼에서 자주 보게될 패턴으로, 보통 글로벌 변수를 선언하고 싶을 경우, 여러분에게 두가지 옵션이 있다:
//      1. .h 파일에 extern을 선언하여 헤더파일을 포함시키거나?
//      2. static 함수를 선언하여, 해당 함수 안에 static 변수를 선언하여, 반환하는 방법 (마치 global 변수로 활용할 수 있음)
//         - 대표적인 예시로 앞서 Singleton 패턴이 되겠다

// 실습:
// - UDlkAssetManager::TestClone 구현
// - bLogAssetLoads의 메모리 주소를 파악
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

UObject* UDlkAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{

	// 해당 함수를 만든 이유는 'synchronous load asset을 불필요하게 하는 것이 있는지 체크하기 위함!'
	if (AssetPath.IsValid())
	{
		// 잠깐 FScopeLogTime을 확인해보자:
		TUniquePtr<FScopeLogTime> LogTimePtr;
		if (ShouldLogAssetLoads())
		{
			// 단순히 로깅하면서, 초단위로 로깅 진행
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronously loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		// 여기서 두가지 분기:
		// 1. AssetManager가 있으면, AssetManager의 StreamableManager를 통해 정적 로딩
		// 2. 아니면, FSoftObjectPath를 통해 바로 정적 로딩
		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}

		// if asset manager is not ready, use LoadObject()
		// - 슥 보면, StaticLoadObject가 보인다: 
		// - 참고로, 항상 StaticLoadObject하기 전에 StaticFindObject를 통해 확인하고 실패하면 진짜 로딩함
		return AssetPath.TryLoad();
	}

	return nullptr;
}

/**
 * member methods
 */
UDlkAssetManager::UDlkAssetManager()
// 항상 Constructor를 까먹지 않도록 하자!
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
	// 오버라이드할 경우, Super의 호출은 꼭 까먹지 말자
	Super::StartInitialLoading();

	// DlkGameplayTags 초기화
	// - Lyra와의 차이점을 한번 보고 가자:
	//   - STARTUP_JOB() 매크로를 활용하고 있으나, 현재 우리는 로딩과 관계 없으므로 간단하게 구현
	FDlkGameplayTags::InitializeNativeTags();
}
