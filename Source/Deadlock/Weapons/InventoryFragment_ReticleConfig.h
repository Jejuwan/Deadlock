#pragma once

#include "Containers/Array.h"
#include "Templates/SubclassOf.h"
#include "Deadlock/Inventory/DlkInventoryItemDefinition.h"
#include "InventoryFragment_ReticleConfig.generated.h"

/** forward declaration */
class UDlkReticleWidgetBase;

UCLASS()
class UDlkInventoryFragment_ReticleConfig : public UDlkInventoryItemFragment
{
	GENERATED_BODY()
public:
	/** ���⿡ ���յ� ReticleWidget ������ ������ �ִ� Fragment */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Reticle)
	TArray<TSubclassOf<UDlkReticleWidgetBase>> ReticleWidgets;
};