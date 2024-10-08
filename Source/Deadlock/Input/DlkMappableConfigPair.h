// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DlkMappableConfigPair.generated.h"

/**
 * 
 */
 /** forward declaration */
class UPlayerMappableInputConfig;

USTRUCT()
struct FDlkMappableConfigPair
{
	GENERATED_BODY()
public:
	FDlkMappableConfigPair() = default;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	UPROPERTY(EditAnywhere)
	bool bShouldActivateAutomatically = true;
};