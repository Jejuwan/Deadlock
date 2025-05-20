#Unreal GAS 기반 MOBA 슈터

### 프로젝트 소개
Unreal Engine의 Lyra 샘플과 Deadlock 게임 구조를 참고해 제작한 MOBA 스타일 슈터 게임입니다. GAS를 활용한 능력 시스템과 컴포넌트 기반 설계를 중점적으로 구현했습니다.

### 사용 기술
- Unreal Engine 5, C++
- Gameplay Ability System, Common UI, Game Feature Plugin

### 핵심 코드 예시

```cpp
void UDlkAbilitySystemComponent::InitAbilities(const FDlkAbilitySet* AbilitySet)
{
    for (const auto& Ability : AbilitySet->Abilities)
    {
        GiveAbility(FGameplayAbilitySpec(Ability.AbilityClass, Ability.Level));
    }
}
```
**설명**: 캐릭터나 무기에 등록된 AbilitySet의 내용을 GAS에 등록하는 로직입니다.

```cpp
void UMyCameraComponent::SetCameraMode(FName NewMode)
{
    CameraModeStack.Push(NewMode);
    UpdateBlend(CameraModeStack.Top());
}
```
**설명**: 카메라 시야는 스택 구조로 관리되며, 조준 시에는 CM_ThirdPersonADS, 기본 상태에선 CM_ThirdPerson으로 전환됩니다.

```cpp
UGameplayCueManager::ExecuteCue(FGameplayTag CueTag, const FGameplayCueParameters& Params)
```
**설명**: 무기 발사, 타격 등에서 발생하는 이펙트를 Cue Tag와 함께 호출합니다.


## Demo Video
[Youtube](https://www.youtube.com/watch?v=nFBAbuTQhJg)
