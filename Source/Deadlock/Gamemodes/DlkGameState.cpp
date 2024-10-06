// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkGameState.h"
#include "Deadlock/Gamemodes/DlkExperienceManagerComponent.h"

ADlkGameState::ADlkGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UDlkExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
