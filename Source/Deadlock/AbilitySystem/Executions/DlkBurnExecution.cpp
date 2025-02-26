// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkBurnExecution.h"
#include "Deadlock/AbilitySystem/Attributes/DlkCombatSet.h"
#include "Deadlock/AbilitySystem/Attributes/DlkBurntSet.h"

/**
 * �ش� Struct�� ����Ͽ�, FGameplayEffectAttributeCaptureDefinition �ν��Ͻ�ȭ�Ͽ� �����Ѵ�
 */
struct FBurnStatics
{
	/** AttributeSet�� � Attribute�� Capture�� �������� ��� Capture���� ���Ǹ� ��� �ִ� (�ѹ� ���� ����) */
	FGameplayEffectAttributeCaptureDefinition BaseBurnDef;

	FBurnStatics()
	{
		BaseBurnDef = FGameplayEffectAttributeCaptureDefinition(UDlkCombatSet::GetBaseBurnAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FBurnStatics& BurnStatics()
{
	// ��� FBurnStatics�� �����ϴ� ���� �����̴�, �ѹ��� �����ϰ� �����Ѵ�
	static FBurnStatics Statics;
	return Statics;
}

UDlkBurnExecution::UDlkBurnExecution() : Super()
{
	// Source�� (�Է°�) Attribute�� ĸ�ĸ� ��������
	// - CombatSet::BaseBurn�� ���� Burning ���� �����ϰ� ���� Execute�� ��, �ش� ���� �����ͼ� Burnth�� Burning�� �����Ѵ�
	RelevantAttributesToCapture.Add(BurnStatics().BaseBurnDef);
}

void UDlkBurnExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// GameplayEffectSpec�� GameplayEffect�� �ڵ�� �����ϸ� �ȴ�
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	float BaseBurn = 0.0f;
	{
		FAggregatorEvaluateParameters EvaluateParameters;

		// �ش� �Լ� ȣ���� ���� DlkCombatSet�� BaseBurn ���� �����´� (Ȥ�� ���� Modifier �� �����Ǿ� �־��ٸ�, ���� ��� ����� ���´�)
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BurnStatics().BaseBurnDef, EvaluateParameters, BaseBurn);
	}

	// RelevantAttributesCapture�� ���� ���� ���� BaseBurn�� 0.0���ϰ� ���� �ʵ��� �Ѵ� (Burning�̴ϱ�!)
	const float BurningDone = FMath::Max(0.0f, BaseBurn);
	if (BurningDone > 0.0f)
	{
		// GameplayEffectCalculation ����, Modifier�μ�, �߰��Ѵ�:
		// - �ش� Modifier�� CombatSet���� ������ BaseBurn�� Ȱ���Ͽ�, BurnthSet�� Burning�� �߰����ش�
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UDlkBurntSet::GetIncreaseAttribute(), EGameplayModOp::Additive, BurningDone));
	}
}