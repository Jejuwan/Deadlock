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
		const FDlkGameplayTags& GameplayTags = FDlkGameplayTags::Get();

		ComponentManager->RegisterInitState(GameplayTags.InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataAvailable, false, GameplayTags.InitState_Spawned);
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataInitialized, false, GameplayTags.InitState_DataAvailable);
		ComponentManager->RegisterInitState(GameplayTags.InitState_GameplayReady, false, GameplayTags.InitState_DataInitialized);
	}
}

void UDlkGameInstance::Shutdown()
{
	Super::Shutdown();
}
