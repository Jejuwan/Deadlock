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
	// ActorInfo는 AbilitySet에서 GiveAbility() 호출로 설정된다
	// - UGameplayAbility::OnGiveAbility()에서 SetCurrentActorInfo()에서 설정된다
	// - AbilitySystemComponent::GiveAbility()에서 OnGiveAbility() 호출한다
	// - DlkAbilitySet::GiveToAbilitySystem()에서 GiveAbility()를 호출한다
	check(CurrentActorInfo);

	AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	check(AvatarActor);

	UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyAbilityComponent);

	//*** 여기서 Lyra는 샷건 처리와 같은 탄착 처리를 생략하고, 권총으로 진행하였다 (아래의 로직은 간단버전이다)

	// Hit 정보를 계산
	TArray<FHitResult> FoundHits;
	PerformLocalTargeting(FoundHits);

	// GameplayAbilityTargetData는 Server/Client 간 Ability의 공유 데이터로 이해하면 된다:
	// - 허나, 우리는 싱글플레이이므로 Ability의 데이터로 생각하면 되겠다 (현재 큰 의미가 없다고 볼 수 있다)
	FGameplayAbilityTargetDataHandle TargetData;
	TargetData.UniqueId = 0;

	if (FoundHits.Num() > 0)
	{
		// Cartridge란 일반 권총의 경우, 탄약에 하나의 총알이 들어있지만, 샷견의 경우, 탄약에 여러개의 총알이 있고, **탄약을 카트리지로 생각**하면 될 것 같다
		const int32 CartridgeID = FMath::Rand();
		for (const FHitResult& FoundHit : FoundHits)
		{
			// AbilityTargetData에 SingeTargetHit 정보를 담는다
			// - 참고로 TargetData.Add()의 경우, SharedPtr에 넣기 때문에 여기서 new는 크게 신경 안써도 된다
			FDlkGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FDlkGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = FoundHit;
			NewTargetData->CartridgeID = CartridgeID;
			TargetData.Add(NewTargetData);
		}
	}

	// 가공된 AbilityTargetData가 준비되었으므로, OnTargetDataReadyCallback을 호출한다
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

	// Skeletal Mesh Component 가져오기
	if (USkeletalMeshComponent* SkeletalMesh = AvatarPawn->FindComponentByClass<USkeletalMeshComponent>())
	{
		// Socket의 위치 가져오기
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
	// 미구현인거 같다... Weapon 위치가 아닌 그냥 Pawn의 위치를 가져온다...
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	check(AvatarPawn);

	const FVector SourceLoc = AvatarPawn->GetActorLocation();

	// Skeletal Mesh Component 가져오기
	if (USkeletalMeshComponent* SkeletalMesh = AvatarPawn->FindComponentByClass<USkeletalMeshComponent>())
	{
		// Socket의 위치 가져오기
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

	// 우선 SweepRadius 없이 한번 Trace 진행한다 (SweepTrace는 무겁기 때문)
	// - FindFirstPawnHitResult()를 여러번 Trace 진행을 막기 위해, OutHits를 확인해서 APawn 충돌 정보있으면 더이상 Trace하지 않는다
	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		Impact = WeaponTrace(InputData, /*SweepRadius=*/0.0f, bIsSimulated, /*out*/ OutHits);
	}

	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		// 만약 SweepRadius가 0보다 크면, 0.0일때 대비 충돌 가능성이 커지므로 한번 더 Trace 진행
		if (SweepRadius > 0.0f)
		{
			// SweepHits에 Trace의 OutHits 정보를 저장
			TArray<FHitResult> SweepHits;
			Impact = WeaponTrace(InputData, SweepRadius, bIsSimulated, SweepHits);

			// Sphere Trace로 진행한 결과인 SweepHits를 검색하여, Pawn이 있는가 검색
			const int32 FirstPawnIdx = FindFirstPawnHitResult(SweepHits);
			if (SweepHits.IsValidIndex(FirstPawnIdx))
			{
				// 만약 있다면, SweepHits를 FirstPawnIdx까지 순회하며, bBlockingHit와 기존 OutHits에 없을 경우 체크한다
				bool bUseSweepHits = true;
				for (int32 Idx = 0; Idx < FirstPawnIdx; ++Idx)
				{
					const FHitResult& CurHitResult = SweepHits[Idx];

					auto Pred = [&CurHitResult](const FHitResult& Other)
						{
							return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
						};

					// OutHits에 있다면... SweepHits를 OutHits로 업데이트 하지 않는다 (이미 충돌했던 정보가 있으니깐?) (early-out용)
					// - OutHits에 bBlockingHits가 SweepHits로 있음을 알게되었음
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

	// Complex Geometry로 Trace를 진행하며, AvatarActor를 AttachParent를 가지는 오브젝트와의 충돌은 무시한다
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex*/true, /*IgnoreActor=*/GetAvatarActorFromActorInfo());
	TraceParams.bReturnPhysicalMaterial = true;

	// AvatarActor에 부착된 Actors를 찾아 IgnoredActors에 추가한다
	AddAdditionalTraceIgnoreActors(TraceParams);

	// Weapon 관련 Collision Channel로 Trace 진행
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
		DrawDebugCapsule(GetWorld(), Center, WeaponHalfHeight, SweepRadius, InputData.Rotation, DrawColor, false, DebugLifeTime); // 시작 위치 캡슐

	}
#endif
	FHitResult Hit(ForceInit);
	if (HitResults.Num() > 0)
	{
		// HitResults 중에 중복(같은) Object의 HitResult 정보를 제거
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

		// Hit의 가장 마지막 값을 Impact로 저장
		Hit = OutHitResults.Last();
	}
	else
	{
		// Hit의 결과 값을 기본 값으로 캐싱
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

		// GetAttachedActors를 한번 보자:
		// - 해당 함수는 Recursively하게 모든 Actors를 추출한다
		// - 근데 왜 앞서 FindFirstPawnHitResult 이건 왜 한단계만 할까? ---
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
		// 현재 Stack에서 InData에서 지금 Local로 Ownership을 가져온다
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

		// CommitAbility 호출로 GE(GameplayEffect)를 처리한다
		// - 현재 아직 우리는 GE에 대해 처리를 진행하지 않을 것이다
		if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			// OnRangeWeaponTargetDataReady BP 노드 호출한다:
			// - 후일 여기서 우리는 GCN(GameplayCueNotify)를 처리할 것이다
			OnMeleeMinionTargetDataReady(LocalTargetDataHandle);
		}
		else
		{
			// CommitAbility가 실패하였으면, EndAbility BP Node 호출한다
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

			// 한단계 AttachParent에 Actor가 Pawn이라면?
			// - 보통 복수개 단계로 AttachParent를 하지 않으므로, AttachParent 대상이 APawn이라고 생각할 수도 있겠다
			if ((HitActor != nullptr) && (HitActor->GetAttachParentActor() != nullptr) && (Cast<APawn>(HitActor->GetAttachParentActor()) != nullptr))
			{
				return Idx;
			}
		}
	}
	return INDEX_NONE;
}
