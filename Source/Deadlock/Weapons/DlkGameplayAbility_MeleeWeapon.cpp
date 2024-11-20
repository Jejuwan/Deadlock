// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkGameplayAbility_MeleeWeapon.h"

#include "AbilitySystemComponent.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "DrawDebugHelpers.h"
#include "DlkMeleeWeaponInstance.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Deadlock/AbilitySystem/DlkGameplayAbilityTargetData_SingleTarget.h"
#include "Deadlock/Physics/DlkCollisionChannels.h"
#include "Deadlock/DlkLogChannels.h"

UDlkGameplayAbility_MeleeWeapon::UDlkGameplayAbility_MeleeWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UDlkGameplayAbility_MeleeWeapon::StartMeleeWeaponTargeting()
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

void UDlkGameplayAbility_MeleeWeapon::PerformLocalTargeting(TArray<FHitResult>& OutHits)
{
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	UDlkMeleeWeaponInstance* WeaponData = GetWeaponInstance();
	if (AvatarPawn && AvatarPawn->IsLocallyControlled() && WeaponData)
	{
		FMeleeWeaponFiringInput InputData;
		InputData.WeaponData = WeaponData;
		InputData.bCanPlayBulletFX = true;

		const FTransform TargetTransform = GetTargetingTransform(AvatarPawn, EDlkAbilityTargetingSource::CameraTowardsFocus);
		// �𸮾��� ForwardVector�� (1, 0, 0) �� EAxis::X�̴�
		// - GetUnitAxis()�� ���캸��
		InputData.AimDir = TargetTransform.GetUnitAxis(EAxis::X);
		InputData.StartTrace = TargetTransform.GetTranslation();
		InputData.EndAim = InputData.StartTrace + InputData.AimDir * WeaponData->MaxDamageRange;

#if 1
		{
			static float DebugThickness = 2.0f;
			DrawDebugLine(GetWorld(), InputData.StartTrace, InputData.StartTrace + (InputData.AimDir * 100.0f), FColor::Yellow, false, 10.0f, 0, DebugThickness);
		}
#endif

		TraceBulletsInCartridge(InputData, OutHits);
	}
}

FTransform UDlkGameplayAbility_MeleeWeapon::GetTargetingTransform(APawn* SourcePawn, EDlkAbilityTargetingSource Source)
{
	check(SourcePawn);
	check(Source == EDlkAbilityTargetingSource::CameraTowardsFocus);

	// ����� �Ʒ� ������ CameraTowardsFocus�� ������ �����̴�:
	// - ������ ������ Lyra�� ����

	AController* Controller = SourcePawn->Controller;
	if (Controller == nullptr)
	{
		return FTransform();
	}

	// �����ѹ��̴�...
	double FocalDistance = 1024.0f;
	FVector FocalLoc;
	FVector CamLoc;
	FRotator CamRot;

	// PlayerController�κ���, Location�� Rotation ������ ������
	APlayerController* PC = Cast<APlayerController>(Controller);
	check(PC);
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	FVector AimDir = CamRot.Vector().GetSafeNormal();
	FocalLoc = CamLoc + (AimDir * FocalDistance);

	// WeaponLoc�� �ƴ� Pawn�� Loc�̴�
	const FVector WeaponLoc = GetWeaponTargetingSourceLocation(WeaponSocketName);
	FVector FinalCamLoc = FocalLoc + (((WeaponLoc - FocalLoc) | AimDir) * AimDir);

#if 1
	{
		// WeaponLoc (��ǻ� ActorLoc)
		DrawDebugPoint(GetWorld(), WeaponLoc, 10.0f, FColor::Red, false, 60.0f);
		//// CamLoc
		//DrawDebugPoint(GetWorld(), CamLoc, 10.0f, FColor::Yellow, false, 60.0f);
		//// FinalCamLoc
		//DrawDebugPoint(GetWorld(), FinalCamLoc, 10.0f, FColor::Magenta, false, 60.0f);

		//// (WeaponLoc - FocalLoc)
		//DrawDebugLine(GetWorld(), FocalLoc, WeaponLoc, FColor::Yellow, false, 60.0f, 0, 2.0f);
		//// (AimDir)
		//DrawDebugLine(GetWorld(), CamLoc, FocalLoc, FColor::Blue, false, 60.0f, 0, 2.0f);

		//// Project Direction Line
		//DrawDebugLine(GetWorld(), WeaponLoc, FinalCamLoc, FColor::Red, false, 60.0f, 0, 2.0f);
	}
#endif

	// Camera -> Focus ��� �Ϸ�
	return FTransform(CamRot, FinalCamLoc);
}

FVector UDlkGameplayAbility_MeleeWeapon::GetWeaponTargetingSourceLocation(FName SocketName) const
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

void UDlkGameplayAbility_MeleeWeapon::TraceBulletsInCartridge(const FMeleeWeaponFiringInput& InputData, TArray<FHitResult>& OutHits)
{
	UDlkMeleeWeaponInstance* WeaponData = InputData.WeaponData;
	check(WeaponData);

	// MaxDamageRange�� ����Ͽ�, EndTrace�� ��������
	const FVector BulletDir = InputData.AimDir;
	const FVector EndTrace = InputData.StartTrace + (BulletDir * WeaponData->MaxDamageRange);

	// HitLocation�� �ʱ�ȭ ������ EndTrace�� ����
	FVector HitLocation = EndTrace;

	// �Ѿ��� �ϳ� Trace �����Ѵ�:m
	// - ����� Lyra�� ���, ���ǰ� ���� Cartridge�� �������� �Ѿ��� ���� ��츦 ó���ϱ� ���� for-loop�� Ȱ���Ͽ�, ������ Bullet�� Trace�Ѵ�
	TArray<FHitResult> AllImpacts;
	FHitResult Impact = DoSingleBulletTrace(InputData.StartTrace, EndTrace, WeaponData->BulletTraceWeaponRadius, /**bIsSimulated=*/ false, /**out*/ AllImpacts);

	const AActor* HitActor = Impact.GetActor();
	if (HitActor)
	{
		if (AllImpacts.Num() > 0)
		{
			OutHits.Append(AllImpacts);
		}

		HitLocation = Impact.ImpactPoint;
	}

	// OutHits�� ��� �ϳ��� �����ϵ���, EndTrace�� Ȱ���Ͽ�, OutHits�� �߰����ش�
	if (OutHits.Num() == 0)
	{
		if (!Impact.bBlockingHit)
		{
			Impact.Location = EndTrace;
			Impact.ImpactPoint = EndTrace;
		}

		OutHits.Add(Impact);
	}
}

FHitResult UDlkGameplayAbility_MeleeWeapon::DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace,
	float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHits) const
{
	FHitResult Impact;

	// �켱 SweepRadius ���� �ѹ� Trace �����Ѵ� (SweepTrace�� ���̱� ����)
	// - FindFirstPawnHitResult()�� ������ Trace ������ ���� ����, OutHits�� Ȯ���ؼ� APawn �浹 ���������� ���̻� Trace���� �ʴ´�
	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		Impact = WeaponTrace(StartTrace, EndTrace, /*SweepRadius=*/0.0f, bIsSimulated, /*out*/ OutHits);
	}

	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		// ���� SweepRadius�� 0���� ũ��, 0.0�϶� ��� �浹 ���ɼ��� Ŀ���Ƿ� �ѹ� �� Trace ����
		if (SweepRadius > 0.0f)
		{
			// SweepHits�� Trace�� OutHits ������ ����
			TArray<FHitResult> SweepHits;
			Impact = WeaponTrace(StartTrace, EndTrace, SweepRadius, bIsSimulated, SweepHits);

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

FHitResult UDlkGameplayAbility_MeleeWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace,float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHitResults) const
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
		GetWorld()->SweepMultiByChannel(HitResults, StartTrace, EndTrace, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(SweepRadius), TraceParams);
	}
	else
	{
		GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace, TraceChannel, TraceParams);
	}

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
		Hit.TraceStart = StartTrace;
		Hit.TraceEnd = EndTrace;
	}

	return Hit;
}

ECollisionChannel UDlkGameplayAbility_MeleeWeapon::DetermineTraceChannel(FCollisionQueryParams& TraceParams,
	bool bIsSimulated) const
{
	return Dlk_TraceChannel_Weapon;
}

void UDlkGameplayAbility_MeleeWeapon::AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const
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

void UDlkGameplayAbility_MeleeWeapon::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,FGameplayTag ApplicationTag)
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
			OnRangeWeaponTargetDataReady(LocalTargetDataHandle);
		}
		else
		{
			// CommitAbility�� �����Ͽ�����, EndAbility BP Node ȣ���Ѵ�
			K2_EndAbility();
		}
	}
}

UDlkMeleeWeaponInstance* UDlkGameplayAbility_MeleeWeapon::GetWeaponInstance()
{
	return Cast<UDlkMeleeWeaponInstance>(GetAssociatedEquipment());
}
