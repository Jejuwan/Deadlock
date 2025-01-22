// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModularAIController.h"
#include "DlkTowerController.generated.h"

namespace ETeamAttitude { enum Type : int; }
struct FGenericTeamId;

class APlayerState;
class UAIPerceptionComponent;
class UObject;
struct FFrame;

/**
 * ALyraPlayerBotController
 *
 *	The controller class used by player bots in this project.
 */
UCLASS(Blueprintable)
class ADlkTowerController : public AModularAIController
{
	GENERATED_BODY()

public:
	ADlkTowerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//Update Team Attitude for the AI
	//UFUNCTION(BlueprintCallable, Category = "Dlk AI Player Controller")
	//void UpdateTeamAttitude(UAIPerceptionComponent* AIPerception);
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void OnUnPossess() override;


	// Attempts to restart this controller (e.g., to respawn it)
	void ServerRestartController();
protected:	
	//~AController interface
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	//~End of AController interface

//private:
//	UPROPERTY()
//	FOnLyraTeamIndexChangedDelegate OnTeamChangedDelegate;
//
//	UPROPERTY()
//	TObjectPtr<APlayerState> LastSeenPlayerState;
};
