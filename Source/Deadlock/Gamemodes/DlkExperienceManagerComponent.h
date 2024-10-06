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
 * - 말 그대로, 해당 component는 game state를 owner로 가지면서, experience의 상태 정보를 가지고 있는 component이다
 * - 뿐만 아니라, manager라는 단어가 포함되어 있듯이, experience 로딩 상태 업데이트 및 이벤트를 관리한다
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
 * 아래의 OnExperienceLoaded에 바인딩하거나, 이미 Experience 로딩이 완료되었다면 바로 호출함
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

	/** Experience의 로딩 상태를 모니터링 */
	EDlkExperienceLoadState LoadState = EDlkExperienceLoadState::Unloaded;

	/** Experience 로딩이 완료된 이후, Broadcasting Delegate */
	FOnDlkExperienceLoaded OnExperienceLoaded;

};
