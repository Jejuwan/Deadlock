// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DlkPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API ADlkPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	ADlkPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
