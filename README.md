## Unreal GAS 기반 MOBA 슈터

### 프로젝트 소개
Unreal Engine의 Lyra 샘플과 Deadlock 게임 구조를 참고해 제작한 MOBA 스타일 슈터 게임입니다. GAS를 활용한 능력 시스템과 컴포넌트 기반 설계를 중점적으로 구현했습니다.

### 사용 기술
- Unreal Engine 5, C++
- Gameplay Ability System, Common UI, Game Feature Plugin

### 핵심 코드 
#### 1. 캐릭터 외형을 컴포넌트화하여  메시 교체가 가능하도록 제작
ControllerComponent_CharacterParts는 내가 고른 코스메틱 데이터를 기억하는 용도이고, PawnComponent_CharacterParts는 현재 보여줄 외형을 장착하는 실행 파트입니다.

#### ControllerComponent_CharacterParts
Controller가 Pawn을 Possess할 때 PawnComponent에 CharacterPart를 추가합니다.
```cpp
if (UDlkPawnComponent_CharacterParts* NewCustomizer = NewPawn ?
NewPawn->FindComponentByClass<UDlkPawnComponent_CharacterParts>() : nullptr)
{
	for (FDlkControllerCharacterPartEntry& Entry : CharacterParts)
	{
		check(!Entry.Handle.IsValid());
		Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
	}
}
```
#### PawnComponent_CharacterParts
PawnComponent에서 ChildActorComponent를 부착하고 CharacterPart의 Class로 설정합니다.
```cpp
UChildActorComponent* PartComponent = NewObject<UChildActorComponent>(OwnerComponent->GetOwner());
PartComponent->SetupAttachment(ComponentToAttachTo, Entry.Part.SocketName);
PartComponent->SetChildActorClass(Entry.Part.PartClass);
PartComponent->RegisterComponent();
```

#### 2. 무기별 스탯, UI 등을 Item Fragment 구조로 모듈화
아이템에 부착하는 일종의 컴포넌트로, 특정 속성을 부여하는 역할을 합니다. ItemFragment는 인스턴스를 직접 구현하지 않고, 대신 DefaultToInstanced와 EditInlineNew 지정자를 사용해 에디터에서 추가만 하면 자동으로 인스턴스가 생성되도록 설계되어 있습니다.
```cpp
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class UDlkInventoryItemFragment : public UObject
{
	GENERATED_BODY()
public:
	/** interface to call when inventory item instance is added to UHakInventoryManagerComponent's InventoryList */
	virtual void OnInstanceCreated(UDlkInventoryItemInstance* Instance) const {}
};
```
UDlkInventoryItemInstance별로 가지고 있는 Fragment를 순회하며 OnInstanceCreated를 호출합니다.
```cpp
for (UDlkInventoryItemFragment* Fragment : GetDefault<UDlkInventoryItemDefinition>(ItemDef)->Fragments)
{
	if (Fragment)
	{
		Fragment->OnInstanceCreated(NewEntry.Instance);
	}
}
```

#### 3. GameFeatureAction_AddWidget으로 UI를 자동으로 추가
AddToWorld에서 UI를 추가합니다. GameFrameworkComponentManager에 HandleActorExtension함수를 바인드합니다.
Widget을 추가하는 대상으로 DlkHUD로 설정하고 DlkHUD를 GameFrameworkComponentManager에 등록해 에디터에서 위젯 추가할 수 있게 합니다.
```cpp
if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
{
	TSoftClassPtr<AActor> HUDActorClass = ADlkHUD::StaticClass();

	TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(
		HUDActorClass,
		UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
		this, &ThisClass::HandleActorExtension, ChangeContext));
	ActiveData.ComponentRequests.Add(ExtensionRequestHandle);
}
```
## 데모 영상
[Youtube](https://www.youtube.com/watch?v=nFBAbuTQhJg)
