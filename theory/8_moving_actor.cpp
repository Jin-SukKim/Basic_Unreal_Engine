/*
    움직이는 액터의 설계

    액터의 틱(Tick) 함수를 사용해 액터의 움직임을 구현한다.

    UE에서 액터의 틱함수는 화면을 만들어내는 렌더링 프레임 단위로 동작한다.
    이전 렌더링 프레임으로부터 현재 렌더링 프레임까지 소요된시간은 Tick 함수의
    인자인 DeltaSeconds를 통해 알 수 있다.

    회전시키고자 할 각속도의 값을 변수로 지정하고, 이를 사용해 분수대를 z축을 기준으로 회전시킨다.
    이 속도 정보는 값 유형이므로 에디터에서 편집 가능하게 UPROPERY 매크로에 EditAnywhere 키워드를 넣는다.

    변수 생성 시 객체 지향 언어 설계의 중요한 원칙 중 하나인 데이터 은닉(information hiding)을 고려해
    설계한다.

    정보 은닉을 위해 private으로 선언하면 컴파일 과정에서 에러가 발생한다.
    향후 언리얼 에디터에서 변수의 값을 변경하려면 접근 권한을 개방해야 한다.
    
    UPROPERTY 매크로에 AllowPrivateAccess라는 META 키워드를 추가하면 에디터에서는
    이를 편집함과 동시에 변수 데이터를 은닉할 수 있게 돼 캡슐화(Encapsulation)가 가능해진다.

    RotateSpeed의 기본값을 생성자에서 초기화하고
    액터의 Tick함수에는 AddActorLocalRotation 함수를 사용해 회전하는 코드를 제작한다.

    UE는 이동과 스케일을 위한 정보에 FVector를 사용하지만 회전에서는 FRotator를 사용한다.
    이 FRotator의 회전 값은 Pitch, Yaw, Roll 순서대로 구성된다.

    UE에서의 회전 축

    Pitch : 좌우를 기준으로 회전 (Y축 회전)
    Yaw : 상하를 기준으로 회전 (Z축 회전)
    Roll : 정면을 기준으로 회전 (X축 회전)

    UE에서 시간은 월드가 관리한다.
    월드에는 시간 관리자(TimeManager)가 있어 다양한 시간 값들을 얻어올 수 있따.

    Tick 함수 인자의 DeltaSeconds 값은 GetWorld()->GetDeltaSeconds() 함수로,
    Tick 함수가 아닌 곳에 프레임 시간을 가져오고자 할 때 유용할 것이다.

    게임 월드에서 제공하는 시간 정보

    1. 게임을 시작한 후 현재까지 경과된 시간 : GetWorld()->GetTimeSeconds()
    2. 사용자가 게임을 중지한 시간을 제외한 경과 시간 : GetWorld()->GetUnpuasedTimeSeconds()

    게임 월드 시간은 현실과 동일한 초 단위로 구성된다.
    하지만 시간의 속도는 조정이 가능하기에 게임 시간과 현실 세계 시간 간에 차이가 발생하는대,
    현실 세계의 시간 단위로 게임 셩과 시간을 알고 싶다면 다음 함수를 사용한다.

    1. 현실 세계의 경과 시간 : GetWorld()->GetRealTimeSeconds()
    2. 사용자가 게임을 중지한 시간을 제외한 현실 세계의 경과 시간 :
        GetWorld()->GetAudioTimeSeconds()
*/