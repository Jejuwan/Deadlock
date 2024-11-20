// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkGameplayAbility_MeleeMinionAttack.h"

#include "AbilitySystemComponent.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Deadlock/AbilitySystem/DlkGameplayAbilityTargetData_SingleTarget.h"
#include "Deadlock/Physics/DlkCollisionChannels.h"
#include "Deadlock/DlkLogChannels.h"
#include "Deadlock/Player/DlkPlayerBotController.h"

UDlkGameplayAbility_MeleeMinionAttack::UDlkGameplayAbility_MeleeMinionAttack(const FObjectInitializer& ObjectInitializer)
{
}

void UDlkGameplayAbility_MeleeMinionAttack::StartMeleeMinionAttackTargeting()
{
	// ActorInfo�� AbilitySet���� GiveAbility() ȣ��� �����ȴ�
	// - UGameplayAbility::OnGiveAbility()���� SetCurrentActorInfo()���� �����ȴ�
	// - AbilitySystemComponent::GiveAbility()���� OnGiveAbility() ȣ���Ѵ�
	// - DlkAbilitySet::GiveToAbilitySystem()���� GiveAbility()�� ȣ���Ѵ�
	check(CurrentActorInfo);

	AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	check(AvatarActor);

	UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyAbilityComponent);

	//*** ���⼭ Lyra�� ���� ó���� ���� ź�� ó���� �����ϰ�, �������� �����Ͽ��� (�Ʒ��� ������ ���ܹ����̴�)

	// Hit ������ ���
	TArray<FHitResult> FoundHits;
	PerformLocalTargeting(FoundHits);

	// GameplayAbilityTargetData�� Server/Client �� Ability�� ���� �����ͷ� �����ϸ� �ȴ�:
	// - �㳪, �츮�� �̱��÷����̹Ƿ� Ability�� �����ͷ� �����ϸ� �ǰڴ� (���� ū �ǹ̰� ���ٰ� �� �� �ִ�)
	FGameplayAbilityTargetDataHandle TargetData;
	TargetData.UniqueId = 0;

	if (FoundHits.Num() > 0)
	{
		// Cartridge�� �Ϲ� ������ ���, ź�࿡ �ϳ��� �Ѿ��� ���������, ������ ���, ź�࿡ �������� �Ѿ��� �ְ�, **ź���� īƮ������ ����**�ϸ� �� �� ����
		const int32 CartridgeID = FMath::Rand();
		for (const FHitResult& FoundHit : FoundHits)
		{
			// AbilityTargetData�� SingeTargetHit ������ ��´�
			// - ����� TargetData.Add()�� ���, SharedPtr�� �ֱ� ������ ���⼭ new�� ũ�� �Ű� �Ƚᵵ �ȴ�
			FDlkGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FDlkGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = FoundHit;
			NewTargetData->CartridgeID = CartridgeID;
			TargetData.Add(NewTargetData);
		}
	}

	// ������ AbilityTargetData�� �غ�Ǿ����Ƿ�, OnTargetDataReadyCallback�� ȣ���Ѵ�
	OnTargetDataReadyCallback(TargetData, FGameplayTag());
}

void UDlkGameplayAbility_MeleeMinionAttack::PerformLocalTargeting(TArray<FHitResult>& OutHits)
{
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	if (AvatarPawn && AvatarPawn->IsLocallyControlled())
	{
		FMeleeMinionAttackInput InputData;

		const FRotator WeaponRot = GetWeaponTargetingSourceRotation(WeaponSocketName);
		const FVector WeaponLoc = GetWeaponTargetingSourceLocation(WeaponSocketName);

		InputData.Rotation = GetWeaponTargetingSourceRotation(WeaponSocketName).Quaternion();
		InputData.StartTrace = GetWeaponTargetingSourceLocation(WeaponSocketName);
		InputData.EndAim = InputData.StartTrace + InputData.Rotation.GetUpVector() * WeaponHalfHeight*2.f;
	

#if 0
		{
			static float DebugThickness = 2.0f;
			DrawDebugLine(GetWorld(), InputData.StartTrace, InputData.StartTrace + (InputData.AimDir * 100.0f), FColor::Yellow, false, 10.0f, 0, DebugThickness);
		}
#endif

		//TraceBulletsInCartridge(InputData, OutHits);
		FHitResult Impact = DoSingleBulletTrace(InputData, WeaponRadius, /**bIsSimulated=*/ false, /**out*/ OutHits);


	}
}


FRotator UDlkGameplayAbility_MeleeMinionAttack::GetWeaponTargetingSourceRotation(FName SocketName) const
{
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	check(AvatarPawn);

	const FRotator SourceTrans = AvatarPawn->GetActorRotation();

	// Skeletal Mesh Component ��������
	if (USkeletalMeshComponent* SkeletalMesh = AvatarPawn->FindComponentByClass<USkeletalMeshComponent>())
	{
		// Socket�� ��ġ ��������
		if (SkeletalMesh->DoesSocketExist(SocketName))
		{
			return SkeletalMesh->GetSocketRotation(SocketName);
		}
		else
		{
			UE_LOG(LogDlk, Warning, TEXT("Socket %s does not exist"), *SocketName.ToString());
		}
	}
	return SourceTrans;
}

FVector UDlkGameplayAbility_MeleeMinionAttack::GetWeaponTargetingSourceLocation(FName SocketName) const
{
	// �̱����ΰ� ����... Weapon ��ġ�� �ƴ� �׳� Pawn�� ��ġ�� �����´�...
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	check(AvatarPawn);

	const FVector SourceLoc = AvatarPawn->GetActorLocation();

	// Skeletal Mesh Component ��������
	if (USkeletalMeshComponent* SkeletalMesh = AvatarPawn->FindComponentByClass<USkeletalMeshComponent>())
	{
		// Socket�� ��ġ ��������
		if (SkeletalMesh->DoesSocketExist(SocketName))
		{
			return SkeletalMesh->GetSocketLocation(SocketName);
		}
		else
		{
			UE_LOG(LogDlk, Warning, TEXT("Socket %s does not exist"), *SocketName.ToString());
		}
	}
	return SourceLoc;
}


FHitResult UDlkGameplayAbility_MeleeMinionAttack::DoSingleBulletTrace(const FMeleeMinionAttackInput& InputData,
	float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHits) const
{
	FHitResult Impact;

	// �켱 SweepRadius ���� �ѹ� Trace �����Ѵ� (SweepTrace�� ���̱� ����)
	// - FindFirstPawnHitResult()�� ������ Trace ������ ���� ����, OutHits�� Ȯ���ؼ� APawn �浹 ���������� ���̻� Trace���� �ʴ´�
	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		Impact = WeaponTrace(InputData, /*SweepRadius=*/0.0f, bIsSimulated, /*out*/ OutHits);
	}

	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		// ���� SweepRadius�� 0���� ũ��, 0.0�϶� ��� �浹 ���ɼ��� Ŀ���Ƿ� �ѹ� �� Trace ����
		if (SweepRadius > 0.0f)
		{
			// SweepHits�� Trace�� OutHits ������ ����
			TArray<FHitResult> SweepHits;
			Impact = WeaponTrace(InputData, SweepRadius, bIsSimulated, SweepHits);

			// Sphere Trace�� ������ ����� SweepHits�� �˻��Ͽ�, Pawn�� �ִ°� �˻�
			const int32 FirstPawnIdx = FindFirstPawnHitResult(SweepHits);
			if (SweepHits.IsValidIndex(FirstPawnIdx))
			{
				// ���� �ִٸ�, SweepHits�� FirstPawnIdx���� ��ȸ�ϸ�, bBlockingHit�� ���� OutHits�� ���� ��� üũ�Ѵ�
				bool bUseSweepHits = true;
				for (int32 Idx = 0; Idx < FirstPawnIdx; ++Idx)
				{
					const FHitResult& CurHitResult = SweepHits[Idx];

					auto Pred = [&CurHitResult](const FHitResult& Other)
						{
							return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
						};

					// OutHits�� �ִٸ�... SweepHits�� OutHits�� ������Ʈ ���� �ʴ´� (�̹� �浹�ߴ� ������ �����ϱ�?) (early-out��)
					// - OutHits�� bBlockingHits�� SweepHits�� ������ �˰ԵǾ���
					if (CurHitResult.bBlockingHit && OutHits.ContainsByPredicate(Pred))
					{
						bUseSweepHits = false;
						break;
					}
				}

				// SweepHits
				if (bUseSweepHits)
				{
					OutHits = SweepHits;
				}
			}
		}
	}

	return Impact;
}

FHitResult UDlkGameplayAbility_MeleeMinionAttack::WeaponTrace(const FMeleeMinionAttackInput& InputData,float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHitResults) const
{
	TArray<FHitResult> HitResults;

	// Complex Geometry�� Trace�� �����ϸ�, AvatarActor�� AttachParent�� ������ ������Ʈ���� �浹�� �����Ѵ�
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex*/true, /*IgnoreActor=*/GetAvatarActorFromActorInfo());
	TraceParams.bReturnPhysicalMaterial = true;

	// AvatarActor�� ������ Actors�� ã�� IgnoredActors�� �߰��Ѵ�
	AddAdditionalTraceIgnoreActors(TraceParams);

	// Weapon ���� Collision Channel�� Trace ����
	const ECollisionChannel TraceChannel = DetermineTraceChannel(TraceParams, bIsSimulated);
	if (SweepRadius > 0.0f)
	{
		GetWorld()->SweepMultiByChannel(HitResults, InputData.StartTrace, InputData.EndAim, InputData.Rotation, TraceChannel, FCollisionShape::MakeSphere(SweepRadius), TraceParams);
	}
	else
	{
		GetWorld()->LineTraceMultiByChannel(HitResults, InputData.StartTrace, InputData.EndAim, TraceChannel, TraceParams);
	}

#if 1
	{
		FVector Center = (InputData.StartTrace + InputData.EndAim) /2.f;
		FColor DrawColor = HitResults.IsEmpty() ? FColor::Red : FColor::Green;
		float DebugLifeTime = 3.0f;
		DrawDebugCapsule(GetWorld(), Center, WeaponHalfHeight, SweepRadius, InputData.Rotation, DrawColor, false, DebugLifeTime); // ���� ��ġ ĸ��

	}
#endif
	FHitResult Hit(ForceInit);
	if (HitResults.Num() > 0)
	{
		// HitResults �߿� �ߺ�(����) Object�� HitResult ������ ����
		for (FHitResult& CurHitResult : HitResults)
		{
			auto Pred = [&CurHitResult](const FHitResult& Other)
				{
					return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
				};

			if (!OutHitResults.ContainsByPredicate(Pred))
			{
				OutHitResults.Add(CurHitResult);
			}
		}

		// Hit�� ���� ������ ���� Impact�� ����
		Hit = OutHitResults.Last();
	}
	else
	{
		// Hit�� ��� ���� �⺻ ������ ĳ��
		Hit.TraceStart = InputData.StartTrace;
		Hit.TraceEnd = InputData.EndAim;
	}

	return Hit;
}

ECollisionChannel UDlkGameplayAbility_MeleeMinionAttack::DetermineTraceChannel(FCollisionQueryParams& TraceParams,
	bool bIsSimulated) const
{
	return Dlk_TraceChannel_Weapon;
}

void UDlkGameplayAbility_MeleeMinionAttack::AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const
{
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		TArray<AActor*> AttachedActors;

		// GetAttachedActors�� �ѹ� ����:
		// - �ش� �Լ��� Recursively�ϰ� ��� Actors�� �����Ѵ�
		// - �ٵ� �� �ռ� FindFirstPawnHitResult �̰� �� �Ѵܰ踸 �ұ�? ---
		Avatar->GetAttachedActors(AttachedActors);

		TraceParams.AddIgnoredActors(AttachedActors);
	}
}

void UDlkGameplayAbility_MeleeMinionAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* MyAbilitySystemComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyAbilitySystemComponent);

	if (const FGameplayAbilitySpec* AbilitySpec = MyAbilitySystemComponent->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		// ���� Stack���� InData���� ���� Local�� Ownership�� �����´�
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

		// CommitAbility ȣ��� GE(GameplayEffect)�� ó���Ѵ�
		// - ���� ���� �츮�� GE�� ���� ó���� �������� ���� ���̴�
		if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			// OnRangeWeaponTargetDataReady BP ��� ȣ���Ѵ�:
			// - ���� ���⼭ �츮�� GCN(GameplayCueNotify)�� ó���� ���̴�
			OnMeleeMinionTargetDataReady(LocalTargetDataHandle);
		}
		else
		{
			// CommitAbility�� �����Ͽ�����, EndAbility BP Node ȣ���Ѵ�
			K2_EndAbility();
		}
	}
}


int32 UDlkGameplayAbility_MeleeMinionAttack::FindFirstPawnHitResult(const TArray<FHitResult>& HitResults) const
{
	for (int32 Idx = 0; Idx < HitResults.Num(); ++Idx)
	{
		const FHitResult& CurHitResult = HitResults[Idx];
		if (CurHitResult.HitObjectHandle.DoesRepresentClass(APawn::StaticClass()))
		{
			return Idx;
		}
		else
		{
			AActor* HitActor = CurHitResult.HitObjectHandle.FetchActor();

			// �Ѵܰ� AttachParent�� Actor�� Pawn�̶��?
			// - ���� ������ �ܰ�� AttachParent�� ���� �����Ƿ�, AttachParent ����� APawn�̶�� ������ ���� �ְڴ�
			if ((HitActor != nullptr) && (HitActor->GetAttachParentActor() != nullptr) && (Cast<APawn>(HitActor->GetAttachParentActor()) != nullptr))
			{
				return Idx;
			}
		}
	}
	return INDEX_NONE;
}
