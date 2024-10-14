#include "DlkGameFeaturePolicy.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_AddGameplayCuePath.h"
#include "GameFeatureData.h"
#include "GameplayCueSet.h"
#include "Deadlock/AbilitySystem/DlkGameplayCueManager.h"

UDlkGameplayFeaturePolicy::UDlkGameplayFeaturePolicy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

void UDlkGameplayFeaturePolicy::InitGameFeatureManager()
{
	// GameFeature_AddGameplayCuePaths�� ���
	Observers.Add(NewObject<UDlkGameFeature_AddGameplayCuePaths>());

	// Observers�� ��ȸ�ϸ�, GameFeaturesSubsystem�� Observers�� ��ȸ�ϸ� ���
	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.AddObserver(Observer);
	}

	// �ռ�, Subsystem�� �̹� Observer�� ��������Ƿ�, GameFeatureManager�� �ʱ�ȭ�Ǵ� ������ �ݿ��� ����
	Super::InitGameFeatureManager();
}

void UDlkGameplayFeaturePolicy::ShutdownGameFeatureManager()
{
	Super::ShutdownGameFeatureManager();

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.RemoveObserver(Observer);
	}
	Observers.Empty();
}

void UDlkGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
{
	// PluginName�� Ȱ���Ͽ�, PluginRootPath�� ���
	const FString PluginRootPath = TEXT("/") + PluginName;

	// GameFeatureData�� Action�� ��ȸ: 
	// - ���� ���, ShooterCore�� GameFeatureData�� Action�� �߰��ߴٸ�, �� �߰��� Action�� ����� ��
	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		// ���߿� �츮�� _AddGameplayCuepath�� ã�Ƽ� ó��
		if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
		{
			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->DirectoryPathsToAdd;

			// GameplayCueManager�� �����ͼ�, GFA�� ��ϵ� DirsToAdd�� �߰��ϸ鼭 GCM�� �����Ͱ� ������Ʈ �ǵ��� ����
			if (UDlkGameplayCueManager* GCM = UDlkGameplayCueManager::Get())
			{
				// RuntimeCueSet�� ������
				UGameplayCueSet* RuntimeGameplayCueSet = GCM->GetRuntimeCueSet();
				const int32 PreInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;

				for (const FDirectoryPath& Directory : DirsToAdd)
				{
					FString MutablePath = Directory.Path;

					// PluginPackagePath�� �ѹ� ������ �� -> ������ ����� MutablePath�� ����
					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);

					// GCM�� RuntimeGameplayCueObjectLibrary�� Path�� �߰�
					GCM->AddGameplayCueNotifyPath(MutablePath, /*bShouldRescanCueAssets=*/false);
				}

				// �ʱ�ȭ �����ְ�! (���� ��ΰ� �߰��Ǿ����� ȣ������� ��)
				if (!DirsToAdd.IsEmpty())
				{
					GCM->InitializeRuntimeObjectLibrary();
				}

				// Cue�� ������ Pre�� Post�� ���Ͽ� ������ �޶����ִٸ�, ���������� RefreshGameplayCuePrimaryAsset ȣ��
				const int32 PostInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
				if (PreInitializeNumCues != PostInitializeNumCues)
				{
					// RefreshGameplayCuePrimaryAsset �Լ��� �߰��� GCN�� AssetManager�� ������ִ� ��Ȱ�� ���ش�
					GCM->RefreshGameplayCuePrimaryAsset();
				}
			}
		}
	}
}

void UDlkGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
{
	const FString PluginRootPath = TEXT("/") + PluginName;
	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(GameFeatureData))
		{
			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->DirectoryPathsToAdd;
			if (UDlkGameplayCueManager* GCM = UDlkGameplayCueManager::Get())
			{
				int32 NumRemoved = 0;
				for (const FDirectoryPath& Directory : DirsToAdd)
				{
					FString MutablePath = Directory.Path;
					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
					NumRemoved += GCM->RemoveGameplayCueNotifyPath(MutablePath, /** bShouldRescanCueAssets=*/ false);
				}

				ensure(NumRemoved == DirsToAdd.Num());

				if (NumRemoved > 0)
				{
					GCM->InitializeRuntimeObjectLibrary();
				}
			}
		}
	}
}