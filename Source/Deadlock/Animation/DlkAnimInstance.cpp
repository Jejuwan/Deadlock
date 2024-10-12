// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkAnimInstance.h"
#include "AbilitySystemGlobals.h"

void UDlkAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UDlkAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	// ASC ���� �����ϴ� GameplayTag�� AnimInstance�� ��� Property�� Delegate�� �����Ͽ�, �� ��ȭ�� ���� �ݿ��� �����Ѵ�
	GameplayTagPropertyMap.Initialize(this, ASC);
}
