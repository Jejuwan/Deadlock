// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkExperienceActionSet.h"
#include "GameFeatureAction.h"

UDlkExperienceActionSet::UDlkExperienceActionSet()
{
}

#if WITH_EDITORONLY_DATA
void UDlkExperienceActionSet::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif