// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerController.h"
#include "DlkPlayerController.generated.h"

class ADlkPlayerState;
class UDlkAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class DEADLOCK_API ADlkPlayerController : public AModularPlayerController
{
	GENERATED_BODY()
	
	ADlkPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/**
		* PlayerController interface
		*/
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	/**
	 * member methods
	 */
	ADlkPlayerState* GetDlkPlayerState() const;
	UDlkAbilitySystemComponent* GetDlkAbilitySystemComponent() const;
};
