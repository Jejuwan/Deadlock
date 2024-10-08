// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DlkCameraMode.generated.h"

class UDlkCameraComponent;

struct FDlkCameraModeView
{
	FDlkCameraModeView();

	void Blend(const FDlkCameraModeView& Other, float OtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/**
 * [0,1]�� BlendFunction�� �°� ������� ���� Ÿ��
 */
UENUM(BlueprintType)
enum class EDlkCameraModeBlendFunction : uint8
{
	Linear,
	/**
	 * EaseIn/Out�� exponent ���� ���� �����ȴ�:
	 */
	EaseIn,
	EaseOut,
	EaseInOut,
	COUNT
};

/** Camera Blending ��� ���� */
UCLASS(Abstract, NotBlueprintable)
class UDlkCameraMode : public UObject
{
	GENERATED_BODY()
public:
	UDlkCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * member methods
	 */
	void UpdateCameraMode(float DeltaTime);
	virtual void UpdateView(float DeltaTime);
	void UpdateBlending(float DeltaTime);

	UDlkCameraComponent* GetDlkCameraComponent() const;
	AActor* GetTargetActor() const;
	FVector GetPivotLocation() const;
	FRotator GetPivotRotation() const;


	/**
	 * member variables
	 */

	 /** CameraMode�� ���� ������ CameraModeView */
	FDlkCameraModeView View;

	/** Camera Mode�� FOV */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", Clampmax = "170.0"))
	float FieldOfView;

	/** View�� ���� Pitch [Min, Max] */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
	float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
	float ViewPitchMax;

	/** �󸶵��� Blend�� �������� �ð��� �ǹ� */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	/** �������� Blend �� [0, 1] */
	float BlendAlpha;

	/**
	 * �ش� CameraMode�� ���� Blend ��
	 * �ռ� BlendAlpha�� ���� ���� �����Ͽ� ���� BlendWeight�� ��� (�ڵ带 ����, �����غ���)
	 */
	float BlendWeight;

	/**
	* EaseIn/Out�� ����� Exponent
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	/** Blend function */
	EDlkCameraModeBlendFunction BlendFunction;
};

/** Camera Blending�� ����ϴ� ��ü */
UCLASS()
class UDlkCameraModeStack : public UObject
{
	GENERATED_BODY()
public:
	UDlkCameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * member variables
	 */
	UDlkCameraMode* GetCameraModeInstance(TSubclassOf<UDlkCameraMode>& CameraModeClass);
	void PushCameraMode(TSubclassOf<UDlkCameraMode>& CameraModeClass);
	void EvaluateStack(float DeltaTime, FDlkCameraModeView& OutCameraModeView);
	void UpdateStack(float DeltaTime);
	void BlendStack(FDlkCameraModeView& OutCameraModeView) const;

	/**
* member variables
*/

	 /** ������ CameraMode�� ���� */
	UPROPERTY()
	TArray<TObjectPtr<UDlkCameraMode>> CameraModeInstances;

	/** Camera Matrix Blend ������Ʈ ���� ť */
	UPROPERTY()
	TArray<TObjectPtr<UDlkCameraMode>> CameraModeStack;
};