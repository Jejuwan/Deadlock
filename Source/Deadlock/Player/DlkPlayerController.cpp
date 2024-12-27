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
	// �켱 PostProcessInput()�� ���� ȣ��Ǵ��� Ȯ���غ���:
	// - UPlayerInput::ProcessInputStack()���� ȣ��ȴ�

	if (UDlkAbilitySystemComponent* DlkASC = GetDlkAbilitySystemComponent())
	{
		DlkASC->ProcessAbilityInput (DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

ADlkPlayerState* ADlkPlayerController::GetDlkPlayerState() const
{
	// ECastCheckedType�� NullAllowed�� Null ��ȯ�� �ǵ��� ��� �����ϴ�
	return CastChecked<ADlkPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UDlkAbilitySystemComponent* ADlkPlayerController::GetDlkAbilitySystemComponent() const
{
	const ADlkPlayerState* DlkPS = GetDlkPlayerState();
	return (DlkPS ? DlkPS->GetDlkAbilitySystemComponent() : nullptr);
}
