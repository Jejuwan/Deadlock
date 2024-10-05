// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DlkUserFacingExperience.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UDlkUserFacingExperience();
	/**
 * member variables
 */

 /** the specific map to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	/** the gameplay expierence to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "DlkExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
};
