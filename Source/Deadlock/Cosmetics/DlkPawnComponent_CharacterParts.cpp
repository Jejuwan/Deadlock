// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkPawnComponent_CharacterParts.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Character.h"

bool FDlkCharacterPartList::SpawnActorForEntry(FDlkAppliedCharacterPartEntry& Entry)
{
	bool bCreatedAnyActor = false;
	// ���޵� AppliedCharacterPartEntry�� Part Class�� ����� ���õǾ� �ִٸ�
	if (Entry.Part.PartClass != nullptr)
	{
		// OwnerComponent�� Owner�� ���� World�� ��ȯ
		UWorld* World = OwnerComponent->GetWorld();

		// DlkPawnComponent_CharacterParts�� ��� Component�� ���� ������ �����Ѵ�: 
		// - GetSceneComponentToAttachTo
		if (USceneComponent* ComponentToAttachTo = OwnerComponent->GetSceneComponentToAttachTo())
		{
			// ���� Component�� ComponentToAttachTo�� Bone Ȥ�� SocketName�� ���� ��� ������ Transform�� ����Ѵ�
			const FTransform SpawnTransform = ComponentToAttachTo->GetSocketTransform(Entry.Part.SocketName);

			// �츮�� Actor-Actor�� �����̹Ƿ�, ChildActorComponent�� Ȱ���Ѵ�
			UChildActorComponent* PartComponent = NewObject<UChildActorComponent>(OwnerComponent->GetOwner());
			PartComponent->SetupAttachment(ComponentToAttachTo, Entry.Part.SocketName);
			PartComponent->SetChildActorClass(Entry.Part.PartClass);
			// ������ RegisterComponent�� ���� ���������� RenderWorld�� FScene�� ���� ������ �����Ѵ� (Ȥ�� �����Ѵ�)
			PartComponent->RegisterComponent();

			// ChildActorComponent���� ������ Actor�� ��ȯ�Ͽ�
			if (AActor* SpawnedActor = PartComponent->GetChildActor())
			{
				// �ش� Actor�� Parent�� DlkPawnComponent_CharacterParts�� Owner Actor���� ���� Tick�� ������� �ʵ��� ���������� ���δ�
				if (USceneComponent* SpawnedRootComponent = SpawnedActor->GetRootComponent())
				{
					SpawnedRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
				}
			}

			Entry.SpawnedComponent = PartComponent;
			bCreatedAnyActor = true;
		}
	}

	return bCreatedAnyActor;
}

void FDlkCharacterPartList::DestroyActorForEntry(FDlkAppliedCharacterPartEntry& Entry)
{
	if (Entry.SpawnedComponent)
	{
		Entry.SpawnedComponent->DestroyComponent();
		Entry.SpawnedComponent = nullptr;
	}
}

FDlkCharacterPartHandle FDlkCharacterPartList::AddEntry(FDlkCharacterPart NewPart)
{
	// PawnComponent�� CharacterPartList�� PartHandle�� �����ϰ�, �̸� ControllerComponent_CharacterParts�� �����Ѵ�
	FDlkCharacterPartHandle Result;
	Result.PartHandle = PartHandleCounter++;

	// Authority�� �ִٸ�, AppliedCharacterPartEntry�� Entries�� �߰��Ѵ�
	if (ensure(OwnerComponent && OwnerComponent->GetOwner() && OwnerComponent->GetOwner()->HasAuthority()))
	{
		FDlkAppliedCharacterPartEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Part = NewPart;
		NewEntry.PartHandle = Result.PartHandle;

		// ���⼭ ���� Actor�� �����ϰ�, OwnerComponent�� Owner Actor�� Actor���� RootComponent�� Attach ��Ų��
		if (SpawnActorForEntry(NewEntry))
		{
			// BroadcastChanged�� ����, OwnerComponent���� Owner�� SkeletalMeshComponent�� Ȱ���Ͽ�, Animation �� Physics�� Re-initialize���ش�
			OwnerComponent->BroadcastChanged();
		}
	}

	return Result;
}

void FDlkCharacterPartList::RemoveEntry(FDlkCharacterPartHandle Handle)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FDlkAppliedCharacterPartEntry& Entry = *EntryIt;

		// ������ ���, PartHandle�� Ȱ���Ѵ�
		if (Entry.PartHandle == Handle.PartHandle)
		{
			DestroyActorForEntry(Entry);
		}
	}
}

FGameplayTagContainer FDlkCharacterPartList::CollectCombinedTags() const
{
	FGameplayTagContainer Result;

	// Entries�� ��ȸ�ϸ�,
	for (const FDlkAppliedCharacterPartEntry& Entry : Entries)
	{
		// Part Actor�� �����Ǿ� SpawnedComponent�� ĳ�̵Ǿ� ������
		if (Entry.SpawnedComponent)
		{
			// �ش� Actor�� IGameplayTagAssetInterface�� ���� GameplayTag�� �˻��Ѵ�:
			// - ���� �츮�� TaggedActor�� IGameplayTagAssetInterface�� ��ӹ��� �����Ƿ� �׳� �Ѿ ���̴�
			// - ���� �����е��� �� Part�� ���� GameplayTag�� �ְ� �ʹٸ� �̰� ��ӹ޾� �����ؾ� �Ѵ�:
			//   - ���� ���, Ư�� Lv100�̻� ���� ������ ��� ����� �ʹٸ� �־�߰ڴ�
			if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Entry.SpawnedComponent->GetChildActor()))
			{
				TagInterface->GetOwnedGameplayTags(Result);
			}
		}
	}

	return Result;
}

UDlkPawnComponent_CharacterParts::UDlkPawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
, CharacterPartList(this)
{
}

USkeletalMeshComponent* UDlkPawnComponent_CharacterParts::GetParentMeshComponent() const
{
	// Character�� Ȱ���Ͽ�, �ֻ��� SkeletalMesh�� ��ȯ�Ѵ�
	if (AActor* OwnerActor = GetOwner())
	{
		if (ACharacter* OwningCharacter = Cast<ACharacter>(OwnerActor))
		{
			if (USkeletalMeshComponent* MeshComponent = OwningCharacter->GetMesh())
			{
				return MeshComponent;
			}
		}
	}
	return nullptr;
}

USceneComponent* UDlkPawnComponent_CharacterParts::GetSceneComponentToAttachTo() const
{
	// Parent�� SkeletalMeshComponent�� ������ ��ȯ�ϰ�
	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		return MeshComponent;
	}

	// �׸��� RootComponent�� Ȯ���ϰ�
	if (AActor* OwnerActor = GetOwner())
	{
		return OwnerActor->GetRootComponent();
	}

	// �׳� nullptr ��ȯ
	return nullptr;
}

FGameplayTagContainer UDlkPawnComponent_CharacterParts::GetCombinedTags(FGameplayTag RequiredPrefix) const
{
	// ���� ������ CharacterPartList�� Merge�� Tags�� ��ȯ�Ѵ�
	FGameplayTagContainer Result = CharacterPartList.CollectCombinedTags();
	if (RequiredPrefix.IsValid())
	{
		// ���� GameplayTag�� ���� ���͸��� ���, ���͸��ؼ� �����Ѵ�
		return Result.Filter(FGameplayTagContainer(RequiredPrefix));
	}
	else
	{
		// ���͸��� GameplayTag�� ������ �׳� ��ȯ
		return Result;
	}
}

void UDlkPawnComponent_CharacterParts::BroadcastChanged()
{
	const bool bReinitPose = true;

	// ���� Owner�� SkeletalMeshComponent�� ��ȯ�Ѵ�
	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		// BodyMeshes�� ����, GameplayTag�� Ȱ���Ͽ�, �˸��� SkeletalMesh�� �缳�����ش�
		const FGameplayTagContainer MergedTags = GetCombinedTags(FGameplayTag());
		USkeletalMesh* DesiredMesh = BodyMeshes.SelectBestBodyStyle(MergedTags);

		// SkeletalMesh�� �ʱ�ȭ �� Animation �ʱ�ȭ �����ش�
		MeshComponent->SetSkeletalMesh(DesiredMesh, bReinitPose);

		// PhysicsAsset�� �ʱ�ȭ���ش�
		if (UPhysicsAsset* PhysicsAsset = BodyMeshes.ForcedPhysicsAsset)
		{
			MeshComponent->SetPhysicsAsset(PhysicsAsset, bReinitPose);
		}
	}
}

FDlkCharacterPartHandle UDlkPawnComponent_CharacterParts::AddCharacterPart(const FDlkCharacterPart& NewPart)
{
	return CharacterPartList.AddEntry(NewPart);
}

void UDlkPawnComponent_CharacterParts::RemoveCharacterPart(FDlkCharacterPartHandle Handle)
{
	CharacterPartList.RemoveEntry(Handle);
}