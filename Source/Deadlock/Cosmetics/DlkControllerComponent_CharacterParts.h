// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DlkCharacterPartTypes.h"
#include "Components/ControllerComponent.h"
#include "DlkControllerComponent_CharacterParts.generated.h"

class UDlkPawnComponent_CharacterParts;

/** ControllerComponent가 소유하는 Character Parts */
USTRUCT()
struct FDlkControllerCharacterPartEntry
{
	GENERATED_BODY()

	/** Character Part에 대한 정의(메타 데이터 == MetaData) */
	UPROPERTY(EditAnywhere)
	FDlkCharacterPart Part;

	/** Character Part 핸들 (고유값) - Controller가 Possess하고 있는 Pawn에서 생성한(인스턴스) Character Part 핸들값 */
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

	/** UFUNCTION으로 정의하지 않으면 AddDynamic이 되지 않는다! */
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UPROPERTY(EditAnywhere, Category = Cosmetics)
	TArray<FDlkControllerCharacterPartEntry> CharacterParts;
};