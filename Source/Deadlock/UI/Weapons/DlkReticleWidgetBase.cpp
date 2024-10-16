#include "DlkReticleWidgetBase.h"
#include "Deadlock/Weapons/DlkWeaponInstance.h"
#include "Deadlock/Inventory/DlkInventoryItemInstance.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(DlkReticleWidgetBase)

UDlkReticleWidgetBase::UDlkReticleWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UDlkReticleWidgetBase::InitializeFromWeapon(UDlkWeaponInstance* InWeapon)
{
	WeaponInstance = InWeapon;
	InventoryInstance = nullptr;
	if (WeaponInstance)
	{
		InventoryInstance = Cast<UDlkInventoryItemInstance>(WeaponInstance->GetInstigator());
	}
	OnWeaponInitialized();
}

