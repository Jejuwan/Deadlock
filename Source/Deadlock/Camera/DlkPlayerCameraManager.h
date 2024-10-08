// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "DlkPlayerCameraManager.generated.h"

/**
 * Controller에 바인딩된 CameraManager
 */
#define DLK_CAMERA_DEFAULT_FOV (80.0f)
#define DLK_CAMERA_DEFAULT_PITCH_MIN (-89.0f)
#define DLK_CAMERA_DEFAULT_PITCH_MAX (89.0f)

/**
 * 
 */
UCLASS()
class DEADLOCK_API ADlkPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	ADlkPlayerCameraManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
