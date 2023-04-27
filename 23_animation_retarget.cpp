/*
    애니메이션 리타겟(Animation Retarget)

    점프 기능을 완성을 위해 점프 애니메이션이 있어야 한다.
    이 애니메이션은 기본 3인칭 마네킹에 있는 점프 애니메이션을 가져와
    생성한 캐릭터에 사용해본다.

    다른 스켈레톤 구성을 가진 캐릭터의 애니메이션 교환은 불가능하지만,
    UE는 인간형 캐릭터의 경우 스켈레톤의 구성이 달라도 애니메이션을 교환할 수 있도록
    애니메이션 리타겟(Animation Retarget) 기능을 제공한다.

    애니메이션 리타겟 기능이 동작하려면 애니메이션을 교환할 캐릭터들의
    스켈레톤을 세팅해줘야 한다.
    
    UE5에서는 빠르게 리타겟팅이 가능한 기능을 제공한다.
    원하는 애니메이션에 우클릭해 retarget animation asset을 한 뒤
    열린 창에서 왼쪽에 source 오른쪽에 리타겟팅할 스켈레톤을 지정해주고 IK_Retargeter도 지정해준다.

    더 자세하게 공부하고 싶다면 IK Rig을 먼저 공부해야 된다.
    IK Rig 시스템은 스켈레탈 메시의 포즈를 편집하는 Solver를 interactive하게 생성하는 수단을 제공한다.

    https://docs.unrealengine.com/5.1/ko/animating-characters-and-objects-in-unreal-engine/
    https://docs.unrealengine.com/5.1/ko/unreal-engine-ik-rig/

    https://devjino.tistory.com/276
*/