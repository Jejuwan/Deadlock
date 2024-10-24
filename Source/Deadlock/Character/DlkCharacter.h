// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "AbilitySystemInterface.h"
#include "DlkCharacter.generated.h"

class UDlkPawnExtensionComponent;
class UDlkCameraComponent;
class UDlkHealthComponent;
class UDlkAbilitySystemComponent;

UCLASS()
class DEADLOCK_API ADlkCharacter : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ADlkCharacter(const FObjectInitializer& ObjectInitializer);

	void OnAbilitySystemInitialized();
	void OnAbilitySystemUninitialized();
	/**
 * IAbilitySystemInterface
 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dlk|Character")
	TObjectPtr<UDlkPawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dlk | Character")
	TObjectPtr<UDlkCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dlk|Character")
	TObjectPtr<UDlkHealthComponent> HealthComponent;

private:
	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Dlk|PlayerState")
	TObjectPtr<UDlkAbilitySystemComponent> AbilitySystemComponent;

};
