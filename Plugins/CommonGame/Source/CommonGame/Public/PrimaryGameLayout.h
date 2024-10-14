#pragma once

#include "CommonActivatableWidget.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "PrimaryGameLayout.generated.h"

/** forward declarations */
class UCommonActivatableWidgetContainerBase;

/**
 * 인게임에서 메인 UI의 레이아웃을 담당하는 UMG이다 (Slate vs UMG를 이해하장 - 간단하게 UObject을 기반하는가 아닌가 차이)
 * - PrimaryGameLayout은 플레이어당 하나씩 가지는 최상위 UI 레이아웃으로 이해하면 된다
 * - PrimaryGameLayout는 레이어 기반으로 UI를 관리한다
 */
UCLASS(Abstract)
class COMMONGAME_API UPrimaryGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	/** LayerName에 따른 ActivatableWidgetContainerBase를 가져옴 */
	UCommonActivatableWidgetContainerBase* GetLayerWidget(FGameplayTag LayerName);

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass)
	{
		return PushWidgetToLayerStack<ActivatableWidgetT>(LayerName, ActivatableWidgetClass, [](ActivatableWidgetT&) {});
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "only CommonActivatableWidgets can be used here");

		if (UCommonActivatableWidgetContainerBase* Layer = GetLayerWidget(LayerName))
		{
			return Layer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, InitInstanceFunc);
		}

		return nullptr;
	}

	/** Layer를 추가하며, GameplayTag를 할당한다 */
	UFUNCTION(BlueprintCallable, Category = "Layer")
	void RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget);

	/** 해당 클래스는 Abstract로 설정되었으므로 굳이 FObjectInitializer::Get()할 필요는 없다 */
	UPrimaryGameLayout(const FObjectInitializer& ObjectInitializer);

	/** GameplayTag --- CommonActivatableWidgetContainerBase */
	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
};