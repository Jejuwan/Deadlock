// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkGameState.h"
#include "Deadlock/Gamemodes/DlkExperienceManagerComponent.h"

ADlkGameState::ADlkGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UDlkExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}

void ADlkGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ADlkGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ADlkGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ADlkGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
