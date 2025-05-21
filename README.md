## Unreal GAS 기반 MOBA 슈터

### 프로젝트 소개
Unreal Engine의 Lyra 샘플과 Deadlock 게임 구조를 참고해 제작한 MOBA 스타일 슈터 게임입니다. GAS를 활용한 능력 시스템과 컴포넌트 기반 설계를 중점적으로 구현했습니다.

### 사용 기술
- Unreal Engine 5, C++
- Gameplay Ability System, Common UI, Game Feature Plugin

### 핵심 코드 
#### 1. 캐릭터 외형을 컴포넌트화하여  메시 교체가 가능하도록 제작
ControllerComponent_CharacterParts는 내가 고른 코스메틱 데이터를 기억하는 용도이고, 
PawnComponent_CharacterParts는 현재 보여줄 외형을 장착하는 실행 파트입니다.

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



## 데모 영상
[Youtube](https://www.youtube.com/watch?v=nFBAbuTQhJg)
