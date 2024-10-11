// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkEquipmentDefinition.h"
#include "DlkEquipmentInstance.h"

UDlkEquipmentDefinition::UDlkEquipmentDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 기본값으로, DlkEquipmentInstance로 설정
	InstanceType = UDlkEquipmentInstance::StaticClass();
}
