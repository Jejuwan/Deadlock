// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkEquipmentDefinition.h"
#include "DlkEquipmentInstance.h"

UDlkEquipmentDefinition::UDlkEquipmentDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// �⺻������, DlkEquipmentInstance�� ����
	InstanceType = UDlkEquipmentInstance::StaticClass();
}
