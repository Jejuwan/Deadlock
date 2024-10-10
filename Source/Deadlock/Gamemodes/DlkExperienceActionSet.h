// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DlkExperienceActionSet.generated.h"

class UGameFeatureAction;

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UDlkExperienceActionSet();

	/**
	 * member variables
	 */
	UPROPERTY(EditAnywhere, Category = "Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};
