// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DlkGameState.generated.h"

class UDlkExperienceManagerComponent;
/**
 * 
 */
UCLASS()
class DEADLOCK_API ADlkGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ADlkGameState();

public:
	UPROPERTY()
	TObjectPtr<UDlkExperienceManagerComponent> ExperienceManagerComponent;

};
