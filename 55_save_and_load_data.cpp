/*
    게임 데이터의 저장과 로딩

    UE는 게임의 데이터를 저장하고 불러들이는 기능을 제공한다.
    SaveGame이라는 언리얼 오브젝트를 상속받은 클래스를 설계하고 이를 언리얼이 제공하는
    세이브게임 시스템에 넘겨주면 게임 데이터의 저장과 로딩을 구현할 수 있다.

    언리얼의 SaveGame 시스템은 각 플랫폼별로 알맞은 최적의 장소에 데이터가 저장되며,
    에디터에서 데이터를 저장하는 경우 프로젝트의 Saved 폴더에 SaveGames라는 폴더안에 저장된다.

    게임 Save 기능에는 각 저장 파일에 접근할 수 있는 고유 이름인 슬롯 이름이 필요하다.
    슬롯 이름을 다르게 지정해 Save Data를 여러 개 만들 수 있다.

    처음에는 세이브된 데이터가 없으므로 기본 세이브 데이터를 생성하는 로직을 PlayerState의 InitPlayerData에 구현한다.
    
    55_code.cpp (line 1 ~ 76)

    이제 UI에 초기화된 플레이어의 데이터가 나타난다.

    플레이어에 관련된 데이터가 변경될 때마다 이를 저장하도록 기능을 구현한다.
    최초에 플레이어 데이터를 생성한 후 바로 저장하고, 이후 경험치 변동이 있을 때마다
    저장하는 로직을 생성한다.

    55_code.cpp (line 80 ~ 123)

    언리얼 오브젝트 생성 시 Newobject 명령을 사용하며, NewObject로 생성된 오브젝트는
    더 이상 사용하지 않으면 언리얼 실행 환경의 Garbage Collector가 이를 탐지해
    자동으로 소멸시킨다.
    즉, NewObject로 생성한 언리얼 오브젝트를 삭제하기 위해 delete 키워드를 사용하지 않아도 된다.

    월드에 액터를 생성하는 작업도 언리얼 오브젝트 생성하는 작업이라고 할 수 있지만
    생성할 때 고려할 점들이 많아 UE은 이를 포괄한 SpawnActor라는 API를 제공하고 있다.
    SpawnActor의 로직 내부를 살펴보면 결국 NewObject를 사용해 액터를 생성한다.

    이제 PlayerState의 HighScore 값을 HUD UI에 연동시킨다.

    55_code.cpp (line 125 ~ 139)

    플레이를 눌러 생성된 첫 NPC를 제거한 후 게임을 다시 실행하면, 경험치와 최고점이 보존돼있다.

    저장된 데이터를 없애고 처음부터 다시 시작하고 싶으면 프로젝트의 Saved 폴더 내의 SaveGames 폴더에서
    해당 파일을 삭제해주면 된다.
    여기서 파일명이 Slot 이름과 동일하다.
*/