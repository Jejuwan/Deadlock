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
	/** 무기에 결합된 ReticleWidget 정보를 가지고 있는 Fragment */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Reticle)
	TArray<TSubclassOf<UDlkReticleWidgetBase>> ReticleWidgets;
};