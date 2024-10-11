// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkEquipmentInstance.h"
#include "DlkEquipmentDefinition.h"
#include "GameFramework/Character.h"

UDlkEquipmentInstance::UDlkEquipmentInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

APawn* UDlkEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

void UDlkEquipmentInstance::SpawnEquipmentActors(const TArray<FDlkEquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		// ���� Owner�� Pawn�� RootComponent�� AttachTarget ������� �Ѵ�
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			// ���� ĳ���Ͷ��, SkeletalMeshComponent�� ������ GetMesh�� ��ȯ�Ͽ�, ���⿡ ���δ�
			AttachTarget = Char->GetMesh();
		}

		for (const FDlkEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			// SpawnActorDeferred�� FinishSpawning�� ȣ���ؾ� Spawn�� �ϼ��ȴ� (��, �ۼ��ڿ��� �ڵ�μ� Ownership�� �ִٴ� �ǹ�)
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/true);

			// Actor�� RelativeTransform�� AttachTransform���� ����
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);

			// AttachTarget�� ������ (Actor -> Actor)
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors.Add(NewActor);
		}
	}
}

void UDlkEquipmentInstance::DestroyEquipmentActors()
{
	// ����� �������� �������� Actor Mesh�� �����Ǿ� ���� ���� �ִ�
	// - ���� Lv10�̾�����, ��ü�� ��ü�� ���� �����Ǿ����� ���� �����ϱ�?
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

APawn* UDlkEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void UDlkEquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void UDlkEquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}
