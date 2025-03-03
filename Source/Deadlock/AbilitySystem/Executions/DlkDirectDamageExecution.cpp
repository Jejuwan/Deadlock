// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkDirectDamageExecution.h"
#include "Deadlock/AbilitySystem/Attributes/DlkCombatSet.h"
#include "Deadlock/AbilitySystem/Attributes/DlkHealthSet.h"

/**
 * 해당 Struct를 사용하여, FGameplayEffectAttributeCaptureDefinition 인스턴스화하여 전달한다
 */
struct FDirectDamageStatics
{
	/** AttributeSet의 어떤 Attribute을 Capture할 것인지와 어떻게 Capture할지 정의를 담고 있다 (한번 보고 오자) */
	FGameplayEffectAttributeCaptureDefinition BaseDirectDamageDef;

	FDirectDamageStatics()
	{
		BaseDirectDamageDef = FGameplayEffectAttributeCaptureDefinition(UDlkCombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDirectDamageStatics& DirectDamageStatics()
{
	static FDirectDamageStatics Statics;
	return Statics;
}

UDlkDirectDamageExecution::UDlkDirectDamageExecution() : Super()
{
	// Source로 (입력값) Attribute를 캡쳐를 정의하자
	RelevantAttributesToCapture.Add(DirectDamageStatics().BaseDirectDamageDef);
}

void UDlkDirectDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// GameplayEffectSpec은 GameplayEffect의 핸들로 생각하면 된다
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	float BaseDirectDamage = 0.0f;
	{
		FAggregatorEvaluateParameters EvaluateParameters;

		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DirectDamageStatics().BaseDirectDamageDef, EvaluateParameters, BaseDirectDamage);
	}

	// RelevantAttributesCapture을 통해 최종 계산된 BaseBurn을 0.0이하가 되지 않도록 한다 (Burning이니깐!)
	const float DirectDamageDone = FMath::Max(0.0f, BaseDirectDamage);
	if (BaseDirectDamage > 0.0f)
	{
		// GameplayEffectCalculation 이후, Modifier로서, 추가한다:
		// - 해당 Modifier는 CombatSet에서 가져온 BaseBurn을 활용하여, BurnthSet의 Burning에 추가해준다
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UDlkHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, BaseDirectDamage));
	}
}