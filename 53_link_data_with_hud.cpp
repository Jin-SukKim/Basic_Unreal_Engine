/*
    HUD(Head Up Display)

    데이터를 확인할 수 있는 UI를 화면에 부착하고 캐릭터 정보를 표시하도록 구현해본다.
    플레이어 데이터를 확인할 수 있는 UI를 생성해준다.
    Widget Blueprint를 생성해 플레이어의 이름, 체력, 레벨, 스코어 등을 확인할 수 있게 생성해준다.

    UI 위젯의 기본 데이터 관리와 로직을 담당하도록 UserWidget을 기본 클래스로하는 ABHUDWidget이란 이름의 클래스를 생성한다.

    생성한 UI_HUD 애셋을 열고 Graph 탭에서 ABHUDWidget 클래스를 부모로 설정한다.

    플레이어 컨트롤러에서 해당 위젯을 생성한 후 이를 화면에 띄워준다.
    UI 애셋의 reference를 복사해 클래스 정보를 불러들이고 CreateWidget 함수로 위젯 인스턴슬르 생성해 띄워준다.
    
    code.cpp (ABPlayerController 클래스)

    해당 UI에는 플레이어의 데이터와 캐릭터의 HP 데이터 정보가 함께 표시된다.
    PlayerState와 Character Stat Component 정보를 모두 해당 HUD에 연동해준다.
    먼저 PlayerState에 새로운 Delegate를 정의하고 Player data가 변경될 때
    HUD에 신호를 보내 HUD가 관련 UI 위젯을 업데이트하도록 구현한다.
    
    code.cpp (ABPlayerState.h, ABHUDWidget.h, ABHUDWidget.cpp from line 42)

    이제 PlayerController에서 HUD 위젯과 PlayerState를 연결하고, 캐릭터에서는 HUD 위젯과 CharacterStatComponent를 연결한다.
    
    code.cpp (ABPlayerController.cpp, ABCharacter.cpp from line 146)

    플레이어 데이터에경험치 정보를 표시하도록 기능을 추가한다.
    경험치 획득과 레벨업 기능을 구현한다.

    UI에서는 해당 정보를 ProgressBar로 표시하기에 경험치와 해당 레벨의 최대 경험치의 비율 정보를 구하는 작업이 필요하다.

    CharacterStat에 NPC를 위한 경험치 값 설정,
    PlayerState에 플레이어 경험치 데이터 보관하도록 설계를 확장,
    캐릭터 사망 시 NPC가 플레이어에게 죽는지 검사하고 해당 PlayerController를 통해 PlayerState를 업데이트하는 로직을 추가해줘야 한다.

    대미지 framework에서 PlayerController의 정보는 가해자(Instigator) 인자로 전달되므로 이를 사용하면 수월해진다.

    code.cpp (ABPlayerState class, from line 179)

    코드를 적용하면 경험치 관련 항목이 업데이트 된걸 확인할 수 있다.
*/