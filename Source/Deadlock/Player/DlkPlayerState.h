// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DlkPlayerState.generated.h"

class UDlkPawnData;
class UDlkExperienceDefinition;
class UDlkAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class DEADLOCK_API ADlkPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ADlkPlayerState(const FObjectInitializer& ObjectInitializer);
	/**
	 * AActor's interface
	 */
	virtual void PostInitializeComponents() final;

	/**
	 * member methods
	 */

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void OnExperienceLoaded(const UDlkExperienceDefinition* CurrentExperience);
	void SetPawnData(const UDlkPawnData* InPawnData);
	UDlkAbilitySystemComponent* GetDlkAbilitySystemComponent() const { return AbilitySystemComponent; }

	UPROPERTY()
	TObjectPtr<const UDlkPawnData> PawnData;

	UPROPERTY(VisibleAnywhere, Category = "Dlk|PlayerState")
	TObjectPtr<UDlkAbilitySystemComponent> AbilitySystemComponent;
};
