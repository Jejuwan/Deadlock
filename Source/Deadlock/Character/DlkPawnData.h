// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DlkPawnData.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UDlkPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};
