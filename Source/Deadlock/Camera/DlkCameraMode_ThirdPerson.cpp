// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkCameraMode_ThirdPerson.h"
#include "Curves/CurveVector.h"
#include "Deadlock/DlkLogChannels.h"


UDlkCameraMode_ThirdPerson::UDlkCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer)
{
}

void UDlkCameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

//	UE_LOG(LogDlk, Log, TEXT("FieldOfView: %f"), FieldOfView);

	// TargetOffsetCurve�� �������̵�Ǿ� �ִٸ�, Curve�� ���� �����ͼ� ���� ����
	// - Camera �������� Charater�� ��� �κп� Target���� ���� �����ϴ� ������ �����ϸ� ��
	if (TargetOffsetCurve)
	{
		const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
		View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
	}
}
