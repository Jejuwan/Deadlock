// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ModularGameState.h"
#include "DlkGameState.generated.h"

class UDlkExperienceManagerComponent;
class UDlkAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class DEADLOCK_API ADlkGameState : public AModularGameStateBase
{
	GENERATED_BODY()
public:
	ADlkGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	//~End of AActor interface

public:
	UPROPERTY()
	TObjectPtr<UDlkExperienceManagerComponent> ExperienceManagerComponent;

private:
	// The ability system component subobject for game-wide things (primarily gameplay cues)
	UPROPERTY(VisibleAnywhere, Category = "Dlk|GameState")
	TObjectPtr<UDlkAbilitySystemComponent> AbilitySystemComponent;


};
