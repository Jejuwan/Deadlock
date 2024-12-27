// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkPlayerController.h"
#include "Deadlock/Camera/DlkPlayerCameraManager.h"
#include "Deadlock/AbilitySystem/DlkAbilitySystemComponent.h"
#include "Deadlock/Player/DlkPlayerState.h"

ADlkPlayerController::ADlkPlayerController(const FObjectInitializer& ObjectInitializer)
{
	PlayerCameraManagerClass = ADlkPlayerCameraManager::StaticClass();
}

void ADlkPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ADlkPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(false);
}

void ADlkPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	// 우선 PostProcessInput()가 언제 호출되는지 확인해보자:
	// - UPlayerInput::ProcessInputStack()에서 호출된다

	if (UDlkAbilitySystemComponent* DlkASC = GetDlkAbilitySystemComponent())
	{
		DlkASC->ProcessAbilityInput (DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

ADlkPlayerState* ADlkPlayerController::GetDlkPlayerState() const
{
	// ECastCheckedType의 NullAllowed는 Null 반환을 의도할 경우 유용하다
	return CastChecked<ADlkPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UDlkAbilitySystemComponent* ADlkPlayerController::GetDlkAbilitySystemComponent() const
{
	const ADlkPlayerState* DlkPS = GetDlkPlayerState();
	return (DlkPS ? DlkPS->GetDlkAbilitySystemComponent() : nullptr);
}
