// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "DlkActivatableWidget.generated.h"

/**
 * Input ó�� ��� ����
 */
UENUM(BlueprintType)
enum class EDlkWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu,
};

/**
 * CommonActivatableWidget�� �ּ��� ����, �����ϰ� ����Ǿ� �ִ�.
 * ���ڰ� ������ CommonActivatableWidget�� �ΰ��� Ư���� ������ �ִ�:
 * 1. Widget Layout�� Widget Instance�� �����ϱ� ���� �뵵�� CommonActivatableWidget�� Layout ����:
 *    - ��Ÿ�� Activate/Deactivate
 *    - WidgetTree���� ���Ű� �ƴ� ����/�״�(== Activate/Deactivate)
 * 2. Input(Mouse or Keyboard etc...) ó�� ��� ����
 */
UCLASS()
class DEADLOCK_API UDlkActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UDlkActivatableWidget(const FObjectInitializer& ObjectInitializer);

	/**
	 * UCommonActivatableWidget's interfaces
	 */
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	/** Input ó�� ��� */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EDlkWidgetInputMode InputConfig = EDlkWidgetInputMode::Default;

	/** Mouse ó�� ��� */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
