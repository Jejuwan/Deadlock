// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonSessionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class COMMONUSER_API UCommonSession_HostSessionRequest : public UObject
{
	GENERATED_BODY()
public:
	FString GetMapName() const;

	FString ConstructTravelURL() const;

	UPROPERTY(BlueprintReadWrite, Category = Session)
	FPrimaryAssetId MapID;

	UPROPERTY(BlueprintReadWrite, Category = Session)
	TMap<FString, FString> ExtraArgs;
};

/**
 * UCommonSessionSubsystem�� Deadlock���� ���Ǿ� �ϱ� ������, Module Export�� ������ϰ� �׷��� COMMONUSER_API�� �߰������ �Ѵ�!
 * - �����е��� {ModuleName}_API�� �߰��� ***�ٸ� ��⿡�� ����� ���, �߰����ָ� �ȴ�***
 */
UCLASS()
class COMMONUSER_API UCommonSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UCommonSessionSubsystem() {}

	UFUNCTION(BlueprintCallable, Category = Session)
	void HostSession(APlayerController* HostingPlayer, UCommonSession_HostSessionRequest* Request);

	/**
	 * member variables
	 */
	 /** PendingTravelURL�� ���� ���� ��η� �����ϸ� �ȴ� */
	FString PendingTravelURL;
};