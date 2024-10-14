#pragma once

#include "Containers/Array.h"
#include "GameFeatureStateChangeObserver.h"
#include "GameFeaturesProjectPolicies.h"
#include "UObject/Object.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "DlkGameFeaturePolicy.generated.h"

/**
 * UDlkGameplayFeaturePolicy�� GameFeature Plugin�� Memory�� �ε��ǰų� Active(Ȱ��ȭ)�� �����ϴ� StateMachine�� ����͸� �����ϴ�
 */
UCLASS()
class UDlkGameplayFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
{
	GENERATED_BODY()
public:
	UDlkGameplayFeaturePolicy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	* GameFeaturesProjectPolicies interfaces
	*/
	virtual void InitGameFeatureManager() override;
	virtual void ShutdownGameFeatureManager() override;

	/** Observer�μ� ����� ��ü�� ���� (�Ʒ� DlkGameFeature_AddGameplayCuePaths�� �ϳ��� ����) */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Observers;
};

/**
 * GameFeature Plugin�� Register/Unregister �ܰ迡�� GameplayCuePath�� ��ϵǾ� GameplayCueManager�� ���� GameplayCue ������ ��ĵ�� �� �ֵ��� �ؾ� �Ѵ�:
 * �̸� ���� GameFeatureStateChangeObserver�� Ȱ���Ͽ�, ������ �������� �̸� �����ϰ� �ϵ��� �Ѵ�:
 * - �Ʒ��� �츮�� Override�� �޼��忡�� ������ �� �ִٽ��� GameFeatureStateChangeObserver�� Ȱ���ϸ� �����ϴ�!
 * - �̸� ���� �߰��� ������ �ʿ��ѵ� �̴� UDlkGameplayFeaturePolicy�� ��������.
 *
 * ������, GameFeatureAction���� Registering/Unregistering�� �ֱ��ϴ�:
 * - �㳪, �츮�� �ռ� �����ߴٽ���, ���� Registering/Unregistering ȣ������� �Ѵ�
 *   - Registering/Unregistering �����غ��� ȣ������ ������ �ƹ� �ǹ� ����...
 * - Lyra������ GameplayFeaturePolicy�� �������̵��ϴ� ������� �̸� �����Ͽ���
 */
UCLASS()
class UDlkGameFeature_AddGameplayCuePaths : public UObject, public IGameFeatureStateChangeObserver
{
	GENERATED_BODY()
public:
	/**
	 * IGameFeatureStateChangeObserver interface
	 */
	virtual void OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
	virtual void OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
};