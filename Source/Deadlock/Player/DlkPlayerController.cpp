// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkPlayerController.h"
#include "Deadlock/Camera/DlkPlayerCameraManager.h"

ADlkPlayerController::ADlkPlayerController(const FObjectInitializer& ObjectInitializer)
{
	PlayerCameraManagerClass = ADlkPlayerCameraManager::StaticClass();
}
