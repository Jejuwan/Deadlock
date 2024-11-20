// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkGameplayAbility_FromEquipment.h"
#include "DlkEquipmentInstance.h"
#include "Deadlock/Inventory/DlkInventoryItemInstance.h"

UDlkEquipmentInstance* UDlkGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	// CurrentActorInfo�� AbilitySystemComponent�� CurrentSpecHandle�� Ȱ���Ͽ�, GameplayAbilitySpec�� ������:
	// - CurrentSpecHandle�� SetCurrentActorInfo() ȣ���� ��, Handle ���� �޾Ƽ� �����:
	// - CurrentSpecHandle�� CurrentActorInfo�� ���� ��
	// - FindAbilitySpecFromHandle�� GiveAbility�� ���� ActivatableAbilities�� ��ȸ�Ͽ� GameplayAbilitySpec�� ã�Ƴ�
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		// GameplayAbility_FromEquipment�� EquipmentInstance�κ��� GiveAbility�� ���������Ƿ�, SourceObject�� EquipmentInstance�� ����Ǿ� ����
		return Cast<UDlkEquipmentInstance>(Spec->SourceObject.Get());
	}
	return nullptr;
}

UDlkInventoryItemInstance* UDlkGameplayAbility_FromEquipment::GetAssociatedItem() const
{
		if (UDlkEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		// In Lyra, equipment is equipped by inventory item instance:
		// - so, equipment's instigator should be inventory item instance
		// - otherwise, it will return nullptr by failing casting to HakInventoryItemInstance
		return Cast<UDlkInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}

int32 UDlkGameplayAbility_FromEquipment::FindFirstPawnHitResult(const TArray<FHitResult>& HitResults) const
{
	for (int32 Idx = 0; Idx < HitResults.Num(); ++Idx)
	{
		const FHitResult& CurHitResult = HitResults[Idx];
		if (CurHitResult.HitObjectHandle.DoesRepresentClass(APawn::StaticClass()))
		{
			return Idx;
		}
		else
		{
			AActor* HitActor = CurHitResult.HitObjectHandle.FetchActor();

			// �Ѵܰ� AttachParent�� Actor�� Pawn�̶��?
			// - ���� ������ �ܰ�� AttachParent�� ���� �����Ƿ�, AttachParent ����� APawn�̶�� ������ ���� �ְڴ�
			if ((HitActor != nullptr) && (HitActor->GetAttachParentActor() != nullptr) && (Cast<APawn>(HitActor->GetAttachParentActor()) != nullptr))
			{
				return Idx;
			}
		}
	}
	return INDEX_NONE;
}