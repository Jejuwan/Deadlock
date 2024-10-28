// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "DlkPlayerController.generated.h"

class ADlkPlayerState;
class UDlkAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class DEADLOCK_API ADlkPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
	
	ADlkPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
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
