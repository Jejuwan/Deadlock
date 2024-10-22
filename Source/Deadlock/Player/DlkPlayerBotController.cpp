// Copyright Epic Games, Inc. All Rights Reserved.

#include "DlkPlayerBotController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
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

