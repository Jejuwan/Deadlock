// Copyright Epic Games, Inc. All Rights Reserved.

#include "DlkGameplayEffectContext.h"

#include "Deadlock/AbilitySystem/DlkAbilitySourceInterface.h"
#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(DlkGameplayEffectContext)

class FArchive;

FDlkGameplayEffectContext* FDlkGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FDlkGameplayEffectContext::StaticStruct()))
	{
		return (FDlkGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FDlkGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

//#if UE_WITH_IRIS
//namespace UE::Net
//{
//	// Forward to FGameplayEffectContextNetSerializer
//	// Note: If FDlkGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
//	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(DlkGameplayEffectContext, FGameplayEffectContextNetSerializer);
//}
//#endif

void FDlkGameplayEffectContext::SetAbilitySource(const IDlkAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IDlkAbilitySourceInterface* FDlkGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IDlkAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FDlkGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

