#pragma once

#include "CommonUserWidget.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "DlkReticleWidgetBase.generated.h"

/** forward declarations */
class UDlkWeaponInstance;
class UDlkInventoryItemInstance;

UCLASS(Abstract)
class UDlkReticleWidgetBase : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UDlkReticleWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void InitializeFromWeapon(UDlkWeaponInstance* InWeapon);

	/**
	 * WeaponInstance/InventoryInstance를 상태 추적용으로 캐싱 목적
	 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UDlkWeaponInstance> WeaponInstance;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UDlkInventoryItemInstance> InventoryInstance;
};