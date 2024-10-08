// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DlkCameraMode.h"
#include "Camera/CameraComponent.h"
#include "DlkCameraComponent.generated.h"

class UDlkCameraModeStack;
/**
 * 
 */
 /** template forward declaration */
template <class TClass> class TSubclassOf;

/** (return type, delegate_name) */
DECLARE_DELEGATE_RetVal(TSubclassOf<UDlkCameraMode>, FDlkCameraModeDelegate);

UCLASS()
class DEADLOCK_API UDlkCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
public:
	UDlkCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UDlkCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDlkCameraComponent>() : nullptr); }

	/**
	* member methods
	*/
	AActor* GetTargetActor() const { return GetOwner(); }
	void UpdateCameraModes();

	/**
	 * CameraComponent interface
	 */
	virtual void OnRegister() final;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;

	/**
	 * member variables
	 */
	 /** 카메라의 blending 기능을 지원하는 stack */
	UPROPERTY()
	TObjectPtr<UDlkCameraModeStack> CameraModeStack;

	/** 현재 CameraMode를 가져오는 Delegate */
	FDlkCameraModeDelegate DetermineCameraModeDelegate;
};
