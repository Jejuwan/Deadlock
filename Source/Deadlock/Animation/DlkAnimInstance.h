// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DlkAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly,Category = "Character State Data")
	float GroundDistance = 1.0f;
};
