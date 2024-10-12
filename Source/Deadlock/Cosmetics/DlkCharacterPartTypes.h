// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DlkCharacterPartTypes.generated.h"

USTRUCT(BlueprintType)
struct FDlkCharacterPartHandle
{
	GENERATED_BODY()

	void Reset()
	{
		PartHandle = INDEX_NONE;
	}

	bool IsValid() const
	{
		return PartHandle != INDEX_NONE;
	}

	UPROPERTY()
	int32 PartHandle = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct FDlkCharacterPart
{
	GENERATED_BODY()

	/** ������ ������ Actor�� Class ������ ������ ���� -> Actor BP�� �ϳ��� ������ �����ϸ� �� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PartClass;

	/** Actor�� ��ȯ�Ͽ� ���̴µ� Ư�� Bone�� ���̱� ���� ���� (���� ���ǵǾ� ���� ������ �׳� Root�� ����) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;
};