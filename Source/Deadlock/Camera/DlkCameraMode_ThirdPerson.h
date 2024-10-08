// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DlkCameraMode.h"
#include "DlkCameraMode_ThirdPerson.generated.h"

class UCurveVector;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class DEADLOCK_API UDlkCameraMode_ThirdPerson : public UDlkCameraMode
{
	GENERATED_BODY()
public:
	UDlkCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/**
	* UDlkCameraMode's interface
	*/
	virtual void UpdateView(float DeltaTime) override;

	/**
	 * member variables
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const UCurveVector> TargetOffsetCurve;
};
