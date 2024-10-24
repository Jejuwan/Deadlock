// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkGameState.h"
#include "Deadlock/AbilitySystem/DlkAbilitySystemComponent.h"
#include "Deadlock/Gamemodes/DlkExperienceManagerComponent.h"

ADlkGameState::ADlkGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UDlkAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	ExperienceManagerComponent = CreateDefaultSubobject<UDlkExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}

void ADlkGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ADlkGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/ this, /*Avatar=*/ this);
}

void ADlkGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ADlkGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
