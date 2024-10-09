// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DlkUserFacingExperience.generated.h"

class UCommonSession_HostSessionRequest;
/**
 * 
 */
UCLASS()
class DEADLOCK_API UDlkUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	//UDlkUserFacingExperience();
	/**
 * member variables
 */
 /**
* Map 로딩 및 Experience 전환을 위해, MapID와 ExperienceID를 활용하여, HostSessionRequest 생성
*/
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UCommonSession_HostSessionRequest* CreateHostingRequest() const;
 /** the specific map to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	/** the gameplay expierence to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "DlkExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
};
