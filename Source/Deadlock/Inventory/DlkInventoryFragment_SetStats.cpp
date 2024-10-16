#include "DlkInventoryFragment_SetStats.h"
#include "DlkInventoryItemInstance.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(DlkInventoryFragment_SetStats)

void UDlkInventoryFragment_SetStats::OnInstanceCreated(UDlkInventoryItemInstance* Instance) const
{
	// iterating InitialItemStats and add stat tag to InventoryItemInstance
	for (const auto& InitialItemStat : InitialItemStats)
	{
		Instance->AddStatTagStack(InitialItemStat.Key, InitialItemStat.Value);
	}
}