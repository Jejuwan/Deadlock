// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkGameInstance.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Deadlock/DlkGameplayTags.h"

void UDlkGameInstance::Init()
{
	Super::Init();

	// 앞서 정의한 InitState의 GameplayTags 등록:
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);
	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(DlkGameplayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(DlkGameplayTags::InitState_DataAvailable, false, DlkGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(DlkGameplayTags::InitState_DataInitialized, false, DlkGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(DlkGameplayTags::InitState_GameplayReady, false, DlkGameplayTags::InitState_DataInitialized);
	}
}

void UDlkGameInstance::Shutdown()
{
	Super::Shutdown();
}