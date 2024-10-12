// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "DlkGameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()
public:
	static UDlkGameplayCueManager* Get();

	UDlkGameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void RefreshGameplayCuePrimaryAsset();
};
