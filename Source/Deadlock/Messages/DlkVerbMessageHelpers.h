// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "DlkVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FDlkVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;


UCLASS()
class DEADLOCK_API UDlkVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dlk")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Dlk")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Dlk")
	static FGameplayCueParameters VerbMessageToCueParameters(const FDlkVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Dlk")
	static FDlkVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
