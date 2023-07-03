/*
    게임플레이 제작

    여태까지는 언리얼이 제공하는 기능을 조합해 콘텐츠가 동작하는 방법을 보여주는 프로토타입을 만들었다.

    UE가 제공하는 플레이어 State와 게임 State에 대해 알아보고 게임에 사용할 데이터를 체계적으로 
    관리하도록 게이므이 기능을 구현한다.

    캐릭터의 스테이트 설정

    캐릭터의 기능을 체계적으로 관리하기 위해 캐릭터에도 state machine model을 구현해본다.
    구현할 캐릭터의 state

    1. PREINT state : 캐릭터 생성 전의 state
        기본 캐릭터 애셋이 설정돼 있지만 캐릭터와 UI를 숨겨둔다.
        해당 state에서는 대미지를 입지 않는다.

    2. LOADING state : 선택한 캐릭터 애셋을 로딩하는 state
        게임이 시작된 시점이므로 현재 조종하는 컨트롤러가 AI인지 플레이어인지 구분할 수 있다.
        플레이어 컨트롤러인 경우 애셋 로딩이 완료될 떄까지 캐릭터를 조종하지 못하도록 입력을 비활성화한다.

    3. READY state : 캐릭터 애셋 로딩이 완료된 state
        숨겨둔 캐릭터와 UI를 보여주며, 이때부터는 공격을 받으면 대미지를 입는다.
        플레이어 컨트롤러는 활성화되며, AI 컨트롤러는 behavior tree 로직을 구동해 캐릭터를 동작시킨다.

    4. DEAD state : 캐릭터가 HP를 소진해 사망할 때 발생하는 state
        죽는 애니메이션을 재생하고 UI를 끄는 한편, 충돌 기능을 없애고 대미지를 입지 않도록 설정한다.
        컨트롤러가 플레이어인 경우 입력을 비활성화하고 AI인 경우 Behavior Tree 로직을 중지한다.
        일정 시간이 지난 후에는 플레이어의 경우 레벨을 재시작하고 AI는 레벨에서 퇴장한다.

    캐릭터의 state를 구분하도록 새로운 열거형(enum)을 정의한다.
    선언된 열거형을 블루프린트에서도 사용하기 위해선 UENUM(BlueprintType)을 선언하고 uint8으로
    기반 유형(underlying type)을 지정해야 한다.
    
    새로운 열거형을 선언하면 ABCharacter에 해당 타입의 변수를 선언하고 state machine 제작을 위한 기본 로직과 함수를 선언한다.

    code.cpp (ArenaBattle.h)

    이 게임은 싱글 플레이 게임이라 캐릭터를 주인공과 NPC가 공유해서 사용하고 있다.
    이런 환경에서 캐릭터는 플레이어인지 NPC인지 판별해야 하는데 확실히 파악할 수 있는 시점 중 하나는 BeginPlay이다.
    컨트롤러가 이 함수에서 부착되기 떄문이다.

    code.cpp (ABCharacter.h)

    캐릭터는 PREINT state에서 시작한다.
    게임이 시작돼 BeginPlay 함수를 호출하면 플레이어인지, NPC인지 파악해 저장하는 변수 IsPlayer에 결과를 저장한다.
    이어 선택된 캐릭터 애셋의 로딩을 시작하고 state를 LOADING으로 변경한다.

    로딩이 완료되면 READY state로 변경하고 본격적인 활동을 시작한다.
    활동 중 HP가 0 이하가 되면 DEAD state로 변경한다.

    플레이어가 캐릭터를 조종하는 경우 임시로 4번 INDEX의 캐릭터 애셋을 사용하고,
    AI가 조종하는 경우 캐릭터 애셋 목록중 랜덤으로 고른다.

    code.cpp (ABCharacter.cpp)
    
    state의 기본 구성이 끝나면, state에 맞게 behavior tree 로직을 수동으로 구동하고
    중지할 수 있게 AIConstroller의 구조를 변경한다.

    code.cpp (ABAIController 클래스)

    플레이어가 빙의할 때 발생하는 캐릭터의 PossessedBy 함수는 제거하고 대신 캐릭터의
    READY state에서 이를 구현한다.
    플레이어인 경우 입력을 활성화하고 AI의 경우 Behavior Tree system을 구동하는 로직을 추가한다.
    Dead state에는 플레이어의 경우 입력 처리를 중단하고, AI의 경우 Behavior Tree System을 종료한다.
    그리고 타이머를 발동시켜 사망한 이후에 처리할 로직도 구현한다.

    code.cpp (ABCharacter 클래스 from line 225)

*/