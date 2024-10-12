#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "DlkGameplayAbilityTargetData_SingleTarget.generated.h"

USTRUCT()
struct FDlkGameplayAbilityTargetData_SingleTargetHit : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()
public:
	FDlkGameplayAbilityTargetData_SingleTargetHit()
		: CartridgeID(-1)
	{}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FDlkGameplayAbilityTargetData_SingleTargetHit::StaticStruct();
	}

	/** ź�� ID (īƮ����) */
	UPROPERTY()
	int32 CartridgeID;
};