// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "DlkGameInstance.generated.h"

UENUM(BlueprintType)
enum class EDlkCharacterType : uint8
{
	Manny,
	Quinn
};


/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()
	/**
 * UGameInstance's interfaces
 */
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	void SetCharacterType(EDlkCharacterType type) { CharacterType = type; }
	UFUNCTION(BlueprintCallable)
	EDlkCharacterType GetCharacterType() { return CharacterType; }

	UPROPERTY(EditDefaultsOnly)
	EDlkCharacterType CharacterType;
};
