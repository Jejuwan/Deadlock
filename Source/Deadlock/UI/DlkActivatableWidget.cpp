// Fill out your copyright notice in the Description page of Project Settings.


#include "DlkActivatableWidget.h"

UDlkActivatableWidget::UDlkActivatableWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

TOptional<FUIInputConfig> UDlkActivatableWidget::GetDesiredInputConfig() const
{
	// �ռ� ���� �ߴ� WidgetInputMode�� ����, InputConfig�� �������ش�
	// - ECommonInputMode�� ���� Input�� �帧�� Game/Menu Ȥ�� �Ѵٷ� ���ϴ� ���� �����ϴ�
	// - ���� ���, �츮�� Menu�� ���� ���, ���̻� Game�� Input�� ������ ���� ���� ��� �ſ� �����ϴ�:
	//   - ����غ���, Menu ����ε� Space�� �����ų� MouseClick���� �Ѿ��� �����ٸ�... �츮�� �ǵ��� ��Ȳ�� �ƴ� ���̴�
	switch (InputConfig)
	{
	case EDlkWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case EDlkWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case EDlkWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, GameMouseCaptureMode);
	case EDlkWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}
