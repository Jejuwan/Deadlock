#pragma once

#include "NativeGameplayTags.h"

namespace DlkGameplayTags
{
	DEADLOCK_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	DEADLOCK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	DEADLOCK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	DEADLOCK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	DEADLOCK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);


	DEADLOCK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	DEADLOCK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	DEADLOCK_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);
};
