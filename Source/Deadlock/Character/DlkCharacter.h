// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "AbilitySystemInterface.h"
#include "DlkCharacter.generated.h"

class UDlkPawnExtensionComponent;
class UDlkCameraComponent;
class UDlkHealthComponent;
class UDlkBurntComponent;
class UDlkAbilitySystemComponent;


UENUM(BlueprintType)
enum class EDlkCharacterTeam : uint8
{
	RED,
	BLUE,
	Neutral
};

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
	UFUNCTION(BlueprintCallable, Category = "Dlk|Character")
	UDlkAbilitySystemComponent* GetDlkAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFUNCTION(BlueprintCallable, Category = "Dlk|Character")
	EDlkCharacterTeam GetCharacterTeam() { return CharacterTeam; }
	UFUNCTION(BlueprintCallable, Category = "Dlk|Character")
	FColor GetTeamColor() { return TeamColor; }
	UFUNCTION(BlueprintCallable, Category = "Dlk|Character")
	void SetCharacterTeam(EDlkCharacterTeam type) { CharacterTeam = type; }
	UFUNCTION(BlueprintCallable, Category = "Dlk|Character")
	void SetTeamColor(FColor color);
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dlk|Character")
	TObjectPtr<UDlkBurntComponent> BurntComponent;

protected:
	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	UFUNCTION()
	virtual void OnBurntStarted(AActor* OwningActor);

	UFUNCTION()
	virtual void OnBurntFinished(AActor* OwningActor);

	UFUNCTION(BlueprintCallable, Category = "Dlk|Character")
	void DisableMovementAndCollision();
	void DestroyDueToDeath();
	void UninitAndDestroy();


	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDeathFinished"))
	void K2_OnDeathFinished();
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnTeamColorChanged"))
	void K2_OnTeamColorChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void TakeBurnDamage();
protected:
	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Dlk|PlayerState")
	TObjectPtr<UDlkAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly)
	EDlkCharacterTeam CharacterTeam;

	UPROPERTY(EditDefaultsOnly)
	FColor TeamColor;

};
