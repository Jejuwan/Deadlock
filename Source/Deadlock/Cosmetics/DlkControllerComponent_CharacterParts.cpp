// Fill out your copyright notice in the Description page of Project Settings.

#include "DlkControllerComponent_CharacterParts.h"
#include "DlkPawnComponent_CharacterParts.h"

UDlkControllerComponent_CharacterParts::UDlkControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UDlkControllerComponent_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (AController* OwningController = GetController<AController>())
		{
			OwningController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
		}
	}
}

void UDlkControllerComponent_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllCharacterParts();
	Super::EndPlay(EndPlayReason);
}

PRAGMA_DISABLE_OPTIMIZATION
UDlkPawnComponent_CharacterParts* UDlkControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// �����غ���, �츮�� �ռ� DlkPawnComponent_CharacterParts�� ��ӹ޴� B_MannequinPawnCosmetics�� �̹� B_Hero_ShooterMannequin�� �߰��Ͽ���.
		// B_MannequinPawnCosmetics�� ��ȯ�Ǳ� ����Ѵ�
		return ControlledPawn->FindComponentByClass<UDlkPawnComponent_CharacterParts>();
	}
	return nullptr;
}
PRAGMA_ENABLE_OPTIMIZATION

void UDlkControllerComponent_CharacterParts::AddCharacterPart(const FDlkCharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart);
}

void UDlkControllerComponent_CharacterParts::AddCharacterPartInternal(const FDlkCharacterPart& NewPart)
{
	FDlkControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();
	NewEntry.Part = NewPart;

	if (UDlkPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
	}
}

void UDlkControllerComponent_CharacterParts::RemoveAllCharacterParts()
{
	if (UDlkPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		for (FDlkControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}
	CharacterParts.Reset();
}

void UDlkControllerComponent_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// ���� OldPawn�� ���ؼ��� Character Parts�� ���� ����������
	if (UDlkPawnComponent_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<UDlkPawnComponent_CharacterParts>() : nullptr)
	{
		for (FDlkControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
		}
	}

	// ���ο� Pawn�� ���ؼ� ���� Controller�� ������ �ִ� Character Parts�� �߰�������
	if (UDlkPawnComponent_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<UDlkPawnComponent_CharacterParts>() : nullptr)
	{
		for (FDlkControllerCharacterPartEntry& Entry : CharacterParts)
		{
			check(!Entry.Handle.IsValid());
			Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
		}
	}
}
