# SoulLike

## 소개

언리얼 처음 공부하면서 어떤걸 만들까 생각중에

좋아하는 게임을 조금이나마 만들어보자 해서 만들었습니다.

하지만 전체 파일을 넣기엔 너무 큰 바람에 소스파일만 커밋했습니다.

## 영상
필드전
[![FiledPlay](https://img.youtube.com/vi/IIODzGtT1dk/sddefault.jpg)](https://www.youtube.com/watch?v=IIODzGtT1dk)

보스전
[![BossPlay](https://img.youtube.com/vi/2B34GB5ynyw/sddefault.jpg)](https://www.youtube.com/watch?v=2B34GB5ynyw)

## 설명

1. 플레이어
   - 공격 : 콤보 공격으로 마우스 좌클릭으로 왼쪽 오른쪽 공격
   - 회피 : 원하는 애니메이션이 없어서 약간 스탭을 밟는것으로 회피 구현
   - 스테미너 : 공격과 회피, 그리고 달릴때마다 일정한 스테미너 감소 스테미너가 없을경우 위 행동을 할 수 없음

2. 적
   1. 미니언
       - 공격 : 공격의 종류는 2~3개 있으며, 각 공격마다 쿨타임과 공격한뒤에 다음 공격까지의 시간이 정해져있음
      1. 일반 미니언 (도끼)
        - 가장 기본적인 미니언 플레이어가 앞에 있을 경우 가만히 있음
        2. 총 미니언
        - 일정 거리 안에 들어오면 총 을 쏨
        - 아군 적군 상관없이 데미지가 들어감
        3. 쌍검 미니언
        - 플레이어가 앞에 있을 경우 플레이어 주변을 돎
        - 만약 공격 타이밍이 돌아오면 플레이어 앞까지 접근한뒤 공격

    2. 보스
        - 공격
          - 플레이어와의 거리에 따라 패턴이 달라짐
            - 장거리 : 검은 구체를 날리거나 플레이어 주변에 폭탄을 터트림 그리고 플레이어에게 빠르게 접근함
            - 중거리 폭탄을 터트리거나 땅을 크게 내려쳐 주변에 데미지를 줌
            - 근거리 땅을 내려치거나 근거리에서 망치를 휘두름

## 사용한 에셋

[Target System](https://www.unrealengine.com/marketplace/ko/product/target-system-component-plugin) : 적을 타겟팅 하는 시스템

[Paragon Models](https://www.unrealengine.com/ko/paragon) : 플레이어 및 보스 모델

[Infinity Blade](https://www.unrealengine.com/marketplace/ko/assets?keywords=infinity%20blade) : 맵과 미니언의 모델

## 제작 기간
2020년 3월 8일 ~ 2020년 3월 26일
