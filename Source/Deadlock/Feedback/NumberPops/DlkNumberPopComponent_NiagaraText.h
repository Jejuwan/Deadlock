// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DlkNumberPopComponent.h"

#include "DlkNumberPopComponent_NiagaraText.generated.h"

class UDlkDamagePopStyleNiagara;
class UNiagaraComponent;
class UObject;

UCLASS(Blueprintable)
class UDlkNumberPopComponent_NiagaraText : public UDlkNumberPopComponent
{
	GENERATED_BODY()

public:

	UDlkNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UDlkNumberPopComponent interface
	virtual void AddNumberPop(const FDlkNumberPopRequest& NewRequest) override;
	//~End of UDlkNumberPopComponent interface

protected:
	
	TArray<int32> DamageNumberArray;

	/** Style patterns to attempt to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UDlkDamagePopStyleNiagara> Style;

	//Niagara Component used to display the damage
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
};
