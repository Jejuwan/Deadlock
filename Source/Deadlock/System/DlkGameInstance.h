// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DlkGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkGameInstance : public UGameInstance
{
	GENERATED_BODY()
	/**
 * UGameInstance's interfaces
 */
	virtual void Init() override;
	virtual void Shutdown() override;
};
