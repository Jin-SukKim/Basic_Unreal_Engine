/*
    타이틀 화면의 제작

    관련 UI 애셋을 Book/UI 폴더에 있다.
    UI_Title.uasset 파일을 사용할 것이다.(혹은 Widget Blueprint로 생성해준다.)

    그리고 공백 레벨, 비어있는 Level을 Title이란 이름으로 하나 생성해준다.

    이 레벨을 아무 기능 없이 UI 화면만 띄우는 역할을 수행할 것이다.
    해당 레벨에서 사용할 게임 모드와 UI를 띄울 플레이어 컨트롤러를 제작한다.

    그중 먼저 플레이어 컨트롤러를 제작한다.
    PlayerController 클래스를 상속하는 ABUIPlayerController 클래스를 생성해준다.

    생성한 클래스를 상속받는 블루프린트에서 아프로 띄울 UI의 클래스 값을 에디터에 설정할 수
    있도록 위젯 클래스 속성을 추가하고 EditDefaultOnly 키워드를 지정한다.

    PlayerController의 로직은 게임 시작시 해당 클래스로부터 UI 인스턴스를 생성하고,
    이를 뷰포트에 띄운 후에 입력은 UI에만 전달되록 한다.

    57_code.cpp (line 1 ~ 35)

    C++ PlayerController의 기본 로직을 완성하면 이를 기반으로 한 Blueprint 클래스를 생성한다.
    혹은 C++ 클래스를 우클릭 후 블루프린트 클래스를 생성할수도 있따.

    Blueprint 클래스를 생성할 때 방금 생성한 ABUIPlayerController를 부모 클래스로
    선택해 생성해준다.

    생성된 PlayerController Blueprint를 열어줘 클래스에서 선언해준 UIWidget class 속성에
    UI 애셋인 UI_Title을 지정해준다.

    플레이어 컨트롤러의 세팅이 끝나면 이 Controller를 띄울 게임 모드를 생성한다.
    이번엔 블루프린트를 사용해 간편하게 생성해준다.

    Game Mode Base로 하는 Blueprint 클래스를 생성해준다.
    이 블루프린트에서 Default Pawn Class를 아무 기능 없는 Pawn으로
    Player Controller Class를 방금 생성한 BP_TitleUIPlayerController로 설정해준다.

    저장해준뒤 언리얼 에디터 좌측 위 드랍다운 메뉴에서 
    List of world Blueprint... -> 가장 아래 World Override -> 생성한 Blueprint GameMode
    를 선택해 해당 월드의 게임 모드를 생성한 게임모드로 바꿔준다.

    이후 플레이 하면 타이틀 UI가 화면에 나온다.

    UI 애셋 내에 미리 만들어둔 Blueprint 로직이 있다(없다면 생성해준다.)

    새로 시작하기 버튼 -> 캐릭터 선택을 위한 Select level로 이동
    이어하기 버튼 -> Gameplay level로 이동
    Player1이란 slot의 데이터가 없다면 이어하기 버튼 비활성화

    Select Level이 없으므로 오류가 발생하는데 캐릭터 선택을 담당하는 레벨인
    Select Level을 제작해준다.

    UI 애셋이 있으나 없다면 생성해준다.
    Select Level도 제공되나 없다면 생성해주자.
    Select Level은 중앙에 캐릭터를 두고 캐릭터를 정면으로 바라보게 카메라가 세팅되있다.
    Point Light 두개가 좌우 대각선 위에서 캐릭터를 비추고 Point Light가 캐릭터 아래에 있다.

    이제 Title blueprint와 동일하게 ABUIPlayerController를 상속받은
    BP_SelectUIPlayerController 블루 프린트를 생성하고 UIWidget Class 속성을 UI_Select로 지정한다.

    BP_SelectGameMode라는 게임 모드르 생성하고 PlayerController 클래스와 DefualtPawnClass를
    각각 BP_SelectUIPlayerController와 Pawn으로 지정한다.

    아까처럼 GameMode를 생성한 블루프린트로 바꿔서 확인해준다.

    이제 C++로 각 UI 버튼의 로직을 구현한다.

    UserWidget을 기반으로 하는 ABCharacterSelectWidget 클래스를 생성한다.
    현재 Level에 있는 스켈레탈 메시 액터의 목록을 가져오고 버튼을 누를 때마다
    Skeletal Mesh Component에 지정한 메시를 변경하는 기능을 구현해준다.
    현재 월드에 있는 특정 타입을 상속받은 액터 목록은 TActorIterator<액터 타입> 구분을 사용해
    가져올 수 있다.

    57_code.cpp (line 37 ~ 101)

    NexhCharacter 함수는 Blueprint에서 사용 가능하게 UFUNCTION 매크로를 추가했다.
    C++ 코드에서 버튼 Control과 NextCharacter 함수를 직접 바인딩 했지만
    블루프린트 로직에서도 호출 할 수 있다.

    57_code.cpp (line 103 ~ 230)

    성공적으로 컴파일되면 UI_Select 애셋의 부모 클래스를 ABCharacterSelectWidget으로 변경한다.

    그리고 버튼이 누를 때 캐릭터 애셋이 변경되는지와
    텍스트 박스에 캐릭터가 사용할 이름을 입력하고 생성 버튼을 누르면 게임이 시작되는지 확인한다.

    정상적으로 작동되면 캐릭터가 선택한 캐릭터가 아닌 기본 캐릭터인데
    선택한 캐릭터 정보를 저장하고 Gameplay level에서 이를 로딩하는 기능을 만든다.

    캐릭터 생성 버튼을 누르면 현재 선택한 캐릭터 정보를 세이브 데이터에 저장하고 
    이를 읽어들이는 기능을 구현한다.

    57_code.cpp (line 232 ~ 310)

    시작 UI의 기능을 모두 완성했으면 Project Setting으로 가서 Title Level을 기본 Level로 설정한다.
*/