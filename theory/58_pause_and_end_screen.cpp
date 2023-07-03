/*
    게임의 중지와 결과 화면

    Gameplay Level에게임을잠시 중지하는 UI와 게임 결과를 띄우는 UI를 추가한다.

    UI_Menu와 UI_Result 애셋이 있는데 없다면 생성해준다.

    편의를 위해 두 위젯이 사용하는 버튼은 기능별로 동일한 이름을 가지도록 한다

    위젯에서사용하는버튼의 이름

    1. btnResume : 현재 진행 중인 게임으로 돌아간다.
    2. btnReturnToTile : 타이틀 레벨로 돌아간다.
    3. btnRetryGame : 게임에 재도전한다.

    중지하는 키로는 ESC가 적합하지만 이 키는 에디터에서 게임플레이를 종료하기 위해
    사용중이므로 테스트를 위해 M키를 대신 사용한다.

    GamePause 액션 매칭을 추가해준다.
    액션 매핑을 추가하면 PlayerController에서해당 키를 인식시키는 코드를 추가한다.
    컨트롤러에 매핑을 처리하면 빙의한 폰에 관계엾이 입력을 처리할수 있따.
    
    58_code.cpp (line 1 ~ 58)

    PlayerController의 코드를 완성하면 UI가 공용으로 사용할 기본 클래스를 ABGameplayWidget이란
    이름으로 생성한다.
    이전과 동일하게 UserWIdget을 부모로하는 클래스를 생성한다.

    새로 생성한 클래스를 UI_Menu 애셋의 부모 클래스로 지정해준다.
    UI_Menu 애셋의 그래프 탭에서 부모 클래스를 ABGameplayWidget으로 변경해준다.
    이제 UI 위젯을 초기화하는 시점에서 발생하는 NativeConstruct 함수에서 이름으로
    버튼을 찾고, 해당 이름의 버튼이 존재하면 바인딩하도록 구현해준다.
    
    58_code.cpp (line 64 ~ 129)

    위젯의 기본 뼈대가 완성돼면 M 버튼을 눌렀을 때 메뉴 UI가 나타나도록 기능을 추가한다.
    메뉴 UI가 나오면 고려할 사항들

    1. 게임플레이의 중지
    2. 버튼을 클릭할 수 있도록 마우스 커서를 보여주기
    3. 입력이 게임에 전달되지 않고 UI에만 전달되도록 제어

    PlayerController의 SetPause 함수를 사용하면 플레이를 일시 중지할 수 있다.
    마우스 커서는 PlayerController의 bShowMouseCursor 속성을 true로 설정하면 보이며,
    UI에만 입력을 전달하도록 SetInputMode 함수에 FInputModeUIOnly 클래스를 인자로 넣어준다.

    관련 기능은 앞으로도 재사용할 예정이므로 이 기능을 묶은 함수를 별도로 제작한다.
    
    58_code.cpp (line 131 ~ 219)

    이제 M 버튼을 눌러 UI를 띄워준다.
    메뉴의 버튼을 눌렀을 때의 기능을 구현한다.

    UI 시스템은 RemoveFromParent 함수를 사용해 현재 뷰포트에 띄워진 자신을 제거할 수 있다.
    UI는 GetOwningPlayer 함수를 사용해 현재 자신을 생성하고 관리하는 PlayerController의
    정보를 가져올 수 있다.
    이를 사용해 입력과 게임의 진행을 원래대로 되돌린다.

    58_code.cpp (line 221 ~ 272)

    메뉴 기능이 정상적으로 동작하면 결과 UI를 표시하는 기능을 구현해준다.
    결과 화면 YUI를 표시하기 전에 ABGameplayWidget을 상속받은 ABGameplayResultWidget 클래스를 생성해준다.

    결과 UI에는 게임을 다시 지작하는 버튼과 결과를 보여주는 텍스트블록, 그리고 점수를 보여주는
    텍스트블록의 세 개의 위젯 컨트롤이 있다.
    다시 시작하는 버튼은 부모 클래스인 ABGameplayWidget에서 처리해준다.

    그리고 UI_Result 애셋의 부모클래스로 생성한 클래스를 설정해준다.

    결과 UI는 게임이 종료되면 마지막으로 한 번만 띄워진다.
    UI의 인스턴스는 시작할 때 미리 만들어뒀다가 게임이 종료될 때 만들어진 인스턴스를 뷰포트에 띄우도록 구성한다.

    58_code.cpp (line 273 ~ 326)

    UI 관련 기능을 구현하면 게임플레이가 종료되는 시점을 지정해야한다.

    게임플레이가 종료되는 시점을 두 가지로 플레이어가 죽을 때와 목표를 달성했을때다.
    전자는 게임 클리어하지 못한 경우고 후자는 게임을 클리어된 상황이다.

    게임의 미션은 테스트하기 편하게 두 개의 섹션을 COMPLETE state로 만들어 달성하도록 해준다.
    게임 미션을 달성했는지 여부는 게임의 정보이므로 GameState에 bGameCleared라는 속성을 추가한다.
    GameMode에서 게임의 미션이 달성되면 현재 게임에서 동작하는 모든 폰을 멈추고 bGameCleared 속성을 true로 한다.
    그리고 미션을 달성하거나 죽으면 PlayerContrller의 ShowResultUI 함수를 호출해 결과 UI를 띄운다.

    58_code.cpp (line 328 ~ 420)

    UI 위젯의 NativeContruct 함수는 AddToViewPort 함수가 외부에서 호출될 때 UI 위젯이 초기화되며 호출된다.
    그래서 PlayerController의 ShowResultUI 함수에서 AddToViewport 함수를 호출하기 전에
    미리 UI 위젯이 GameState의 정보를 읽어들일 수 있도록 바인딩을 설정하고
    ABGameplayWidget 클래스에서 아직 구현못한 btnRetryGame 버튼의 기능을 구현해 마무리한다.

    58_code.cpp (line 422 ~ 478)

    플레이어가 죽거나 클리어 조건을 만족하면 UI가 뜬다.
    테스트가 완료되면 ABGameMode의 ScoreToClear 변수 값을 조정해 난이도를 높여준다.

*/  