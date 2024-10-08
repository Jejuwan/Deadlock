// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkPlayerCameraManager.h"

ADlkPlayerCameraManager::ADlkPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DefaultFOV = DLK_CAMERA_DEFAULT_FOV;
	ViewPitchMin = DLK_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = DLK_CAMERA_DEFAULT_PITCH_MAX;
}
