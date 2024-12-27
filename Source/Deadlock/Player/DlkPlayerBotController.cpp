// Copyright Epic Games, Inc. All Rights Reserved.

#include "DlkPlayerBotController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "Deadlock/Character/DlkPawnExtensionComponent.h"
#include "Deadlock/GameModes/DlkGameModeBase.h"
#include "Deadlock/DlkLogChannels.h"
#include "Perception/AIPerceptionComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DlkPlayerBotController)

class UObject;

ADlkPlayerBotController::ADlkPlayerBotController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsPlayerState = true;
	bStopAILogicOnUnposses = false;
}


void ADlkPlayerBotController::ServerRestartController()
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}

	ensure((GetPawn() == nullptr) && IsInState(NAME_Inactive));

	if (IsInState(NAME_Inactive) || (IsInState(NAME_Spectating)))
	{
		ADlkGameModeBase* const GameMode = GetWorld()->GetAuthGameMode<ADlkGameModeBase>();

		if ((GameMode == nullptr) || !GameMode->ControllerCanRestart(this))
		{
			return;
		}

		// If we're still attached to a Pawn, leave it
		if (GetPawn() != nullptr)
		{
			UnPossess();
		}

		// Re-enable input, similar to code in ClientRestart
		ResetIgnoreInputFlags();

		GameMode->RestartPlayer(this);

		if (GetPawn() != nullptr)
		{
			if (UDlkPawnExtensionComponent* PawnExtComponent = GetPawn()->FindComponentByClass<UDlkPawnExtensionComponent>())
			{
				PawnExtComponent->CheckDefaultInitialization();
			}
		}
	}
}

void ADlkPlayerBotController::InitPlayerState()
{
	Super::InitPlayerState();
}

void ADlkPlayerBotController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
}

void ADlkPlayerBotController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void ADlkPlayerBotController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ADlkPlayerBotController::BeginPlay()
{
	Super::BeginPlay();
}

void ADlkPlayerBotController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}

