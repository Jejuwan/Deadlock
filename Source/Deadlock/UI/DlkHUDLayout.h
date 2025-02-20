// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DlkActivatableWidget.h"
#include "DlkHUDLayout.generated.h"

class UCommonActivatableWidget;
/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkHUDLayout : public UDlkActivatableWidget
{
	GENERATED_BODY()
public:

	UDlkHUDLayout(const FObjectInitializer& ObjectInitializer);

	void NativeOnInitialized() override;

protected:
	void HandleEscapeAction();

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;
};
