// Copyright Epic Games, Inc. All Rights Reserved.

#include "DlkBotCreationComponent.h"
#include "DlkGameModeBase.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "DlkExperienceManagerComponent.h"
#include "Deadlock/Character/DlkPawnExtensionComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Deadlock/Character/DlkHealthComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DlkBotCreationComponent)

UDlkBotCreationComponent::UDlkBotCreationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDlkBotCreationComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for the experience load to complete
	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	UDlkExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UDlkExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_LowPriority(FOnDlkExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void UDlkBotCreationComponent::OnExperienceLoaded(const UDlkExperienceDefinition* Experience)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		ServerCreateBots();
	}
#endif
}

void UDlkBotCreationComponent::ServerCreateBots_Implementation()
{
	if (BotControllerClass == nullptr)
	{
		return;
	}

	RemainingBotNames = RandomBotNames;

	// Determine how many bots to spawn
	int32 EffectiveBotCount = NumBotsToCreate;

	// Give the developer settings a chance to override it
	//if (GIsEditor)
	//{
	//	const ULyraDeveloperSettings* DeveloperSettings = GetDefault<ULyraDeveloperSettings>();

	//	if (DeveloperSettings->bOverrideBotCount)
	//	{
	//		EffectiveBotCount = DeveloperSettings->OverrideNumPlayerBotsToSpawn;
	//	}
	//}

	// Give the URL a chance to override it
	if (AGameModeBase* GameModeBase = GetGameMode<AGameModeBase>())
	{
		EffectiveBotCount = UGameplayStatics::GetIntOption(GameModeBase->OptionsString, TEXT("NumBots"), EffectiveBotCount);
	}

	// Create them
	for (int32 Count = 0; Count < EffectiveBotCount; ++Count)
	{
		SpawnOneBot();
	}
}

void UDlkBotCreationComponent::SpawnOneBot()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = GetComponentLevel();
	SpawnInfo.ObjectFlags |= RF_Transient;
	AAIController* NewController = GetWorld()->SpawnActor<AAIController>(BotControllerClass, FVector(5,0,0), FRotator::ZeroRotator, SpawnInfo);
	
	if (NewController != nullptr)
	{
		ADlkGameModeBase* GameMode = GetGameMode<ADlkGameModeBase>();
		check(GameMode);

		if (NewController->PlayerState != nullptr)
		{
			//NewController->PlayerState->SetPlayerName(CreateBotName(NewController->PlayerState->GetPlayerId()));
		}

		//GameMode->GenericPlayerInitialization(NewController);
		GameMode->RestartPlayer(NewController);

		if (NewController->GetPawn() != nullptr)
		{
			if (UDlkPawnExtensionComponent* PawnExtComponent = NewController->GetPawn()->FindComponentByClass<UDlkPawnExtensionComponent>())
			{
				PawnExtComponent->CheckDefaultInitialization();
			}
		}

		SpawnedBotList.Add(NewController);
	}
}