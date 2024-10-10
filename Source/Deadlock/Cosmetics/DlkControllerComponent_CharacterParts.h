// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DlkCharacterPartTypes.h"
#include "Components/ControllerComponent.h"
#include "DlkControllerComponent_CharacterParts.generated.h"

class UDlkPawnComponent_CharacterParts;

/** ControllerComponent�� �����ϴ� Character Parts */
USTRUCT()
struct FDlkControllerCharacterPartEntry
{
	GENERATED_BODY()

	/** Character Part�� ���� ����(��Ÿ ������ == MetaData) */
	UPROPERTY(EditAnywhere)
	FDlkCharacterPart Part;

	/** Character Part �ڵ� (������) - Controller�� Possess�ϰ� �ִ� Pawn���� ������(�ν��Ͻ�) Character Part �ڵ鰪 */
	FDlkCharacterPartHandle Handle;
};


UCLASS(meta = (BlueprintSpawnableComponent))
class UDlkControllerComponent_CharacterParts : public UControllerComponent
{
	GENERATED_BODY()
public:
	UDlkControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UDlkPawnComponent_CharacterParts* GetPawnCustomizer() const;

	UFUNCTION(BlueprintCallable, Category = Cosmetics)
	void AddCharacterPart(const FDlkCharacterPart& NewPart);

	void AddCharacterPartInternal(const FDlkCharacterPart& NewPart);

	void RemoveAllCharacterParts();

	/** UFUNCTION���� �������� ������ AddDynamic�� ���� �ʴ´�! */
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UPROPERTY(EditAnywhere, Category = Cosmetics)
	TArray<FDlkControllerCharacterPartEntry> CharacterParts;
};