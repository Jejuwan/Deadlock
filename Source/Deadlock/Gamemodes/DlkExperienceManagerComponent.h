// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "DlkExperienceManagerComponent.generated.h"

class UDlkExperienceDefinition;

enum class EDlkExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	ExecutingActions,
	Loaded,
	Deactivating,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDlkExperienceLoaded, const UDlkExperienceDefinition*);

/**
 * HakExperienceManagerComponent
 * - �� �״��, �ش� component�� game state�� owner�� �����鼭, experience�� ���� ������ ������ �ִ� component�̴�
 * - �Ӹ� �ƴ϶�, manager��� �ܾ ���ԵǾ� �ֵ���, experience �ε� ���� ������Ʈ �� �̺�Ʈ�� �����Ѵ�
 */
/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
	
public:
	bool IsExperienceLoaded() { return (LoadState == EDlkExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

	/**
 * �Ʒ��� OnExperienceLoaded�� ���ε��ϰų�, �̹� Experience �ε��� �Ϸ�Ǿ��ٸ� �ٷ� ȣ����
 */
	void CallOrRegister_OnExperienceLoaded(FOnDlkExperienceLoaded::FDelegate&& Delegate);

	void ServerSetCurrentExperience(FPrimaryAssetId ExperienceId);
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnExperienceFullLoadCompleted();
	const UDlkExperienceDefinition* GetCurrentExperienceChecked() const;
public:
	UPROPERTY()
	TObjectPtr<const UDlkExperienceDefinition> CurrentExperience;

	/** Experience�� �ε� ���¸� ����͸� */
	EDlkExperienceLoadState LoadState = EDlkExperienceLoadState::Unloaded;

	/** Experience �ε��� �Ϸ�� ����, Broadcasting Delegate */
	FOnDlkExperienceLoaded OnExperienceLoaded;

};
