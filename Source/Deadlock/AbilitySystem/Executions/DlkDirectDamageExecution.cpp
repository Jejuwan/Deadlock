// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkDirectDamageExecution.h"
#include "Deadlock/AbilitySystem/Attributes/DlkCombatSet.h"
#include "Deadlock/AbilitySystem/Attributes/DlkHealthSet.h"

/**
 * �ش� Struct�� ����Ͽ�, FGameplayEffectAttributeCaptureDefinition �ν��Ͻ�ȭ�Ͽ� �����Ѵ�
 */
struct FDirectDamageStatics
{
	/** AttributeSet�� � Attribute�� Capture�� �������� ��� Capture���� ���Ǹ� ��� �ִ� (�ѹ� ���� ����) */
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
	// Source�� (�Է°�) Attribute�� ĸ�ĸ� ��������
	RelevantAttributesToCapture.Add(DirectDamageStatics().BaseDirectDamageDef);
}

void UDlkDirectDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// GameplayEffectSpec�� GameplayEffect�� �ڵ�� �����ϸ� �ȴ�
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	float BaseDirectDamage = 0.0f;
	{
		FAggregatorEvaluateParameters EvaluateParameters;

		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DirectDamageStatics().BaseDirectDamageDef, EvaluateParameters, BaseDirectDamage);
	}

	// RelevantAttributesCapture�� ���� ���� ���� BaseBurn�� 0.0���ϰ� ���� �ʵ��� �Ѵ� (Burning�̴ϱ�!)
	const float DirectDamageDone = FMath::Max(0.0f, BaseDirectDamage);
	if (BaseDirectDamage > 0.0f)
	{
		// GameplayEffectCalculation ����, Modifier�μ�, �߰��Ѵ�:
		// - �ش� Modifier�� CombatSet���� ������ BaseBurn�� Ȱ���Ͽ�, BurnthSet�� Burning�� �߰����ش�
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UDlkHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, BaseDirectDamage));
	}
}