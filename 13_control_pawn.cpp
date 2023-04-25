/*
    폰의 조작

    플레이어의 입력은 UE이 제공하는 가상 입력 설정을 사용하면
    다양한 기기와 멀티 플랫폼에 효과적으로 대응할 수 있다.

    프로젝트 세팅의 입력 설정에서 이를 지정할 수 있으며, Bindings 섹션에서
    Action Mappings와 Axis Mappings라는 두 가지 입력 설정 항목을 볼 수 있다.

    조이스틱에 비유한다면, 축 매핑(Axis Mapping)은 조이스틱 레버를 의미하고
    액션 매핑(Action Mapping)은 조이스틱의 버튼을 의미한다.

    각 입력 설정의 특징

    1. Axis Mapping : 조이스틱 레버의 신호를 설정하는 곳
        레버가 중립 위치에 있으며 0의 값을, 끝 위치에 있으며 설정된 스케일 값에
        따라 1 혹은 -1의 값을 게임 로직에 전달한다.

        마네킹 캐릭터에서 사용하기 위해 이미 설정된 MoveRight 입력 설정의 경우
        A 키를 누르면 -1의 값이, D 키를 누르면 1의 값이 발생하고 아무 입력 없을시 0이
        지속적으로 발생한다.

    2. Action Mappings : 조이스틱 버튼의 신호를 설정하는 곳이다.
        Axis Mapping와 달리 UE은 버튼을 눌렀을 떄와 땔 떄만 신호를 전달한다.

    현재 UE5에서는 향상된 입력(Enhanced Input)을 제공해 Content brower에 우클릭 혹은 +Add 버튼을
    활용해 Input Action과 Input Mapping Context를 생성해서 사용해야 한다.

    우클릭/Add버튼 -> input -> input action/input mapping context

    이걸로 입력 설정을 만들면 폰의 게임 로직에서 이를 사용하도록 코드를 작성한다.

    InputComponent로 입력 설정을 처리할 수 있다.
    InputComponent를 사용해 폰의 멤버 함수와 입력 설정을 연결(Binding)시키면,
    입력 신호는 자동으로 폰의 멤버 함수의 인자로 전달된다.
    이를 연결하는 곳이 폰의 SetupInputComponent 함수다.

    InputComponent는 BindAxis와 BindAction이라는 두 가지 함수를 제공한다.
    먼저 폰이 레버 신호를 받을 수 있도록 BindAxis 함수를 사용해 입력 설정의 이름과 이와 연동할
    언리얼 오브젝트 인스턴스의 함수포인터를 지정한다.

    EnhancedInput을 사용해주기 위해 Build.cs 파일에 "EnhancedInput" 모듈을 더해준다.

    https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/ProgrammingWithCPP/ModuleQuickStart/

    https://www.youtube.com/watch?v=fW1pXOAIviw&ab_channel=DruidMechanics

    폰 무브먼트 컴포넌트의 AddMovementInput 함수를 사용해 폰을 움직인다.
    이 함수에는 추가로 이동할 방향을 WorldDirection에 지정해줘야한다.
    월드 좌표계를 기준으롷 하는 방향 벡터 데이터를 전달해준다.

    월드 좌표계 기준으로 액터의 전진 방향의 벡터 데이터는 GetActorForwardVector 함수를
    좌우는 GetActorRightVect()를 사용한다.

    폰을 조종하기 위한 입력 로직은 폰 클래스에 구현하는 것이 일반적이다.
    하지만 UE의 입력 시스템은 중간에 플레이어 컨트롤러를 거쳐서 폰에 전달된다.
    만일 플레이어 컨트롤러에 특정 입력 처리하는 코드를 구현하면
    해당 입력은 플레이어 컨트롤러에서 필터링돼 폰에 더 이상 전달되지 않는다.

    플레이어 컨트롤러에서 추가로 고려할 부분은 화면에 관련된 인터페이스 기능이다.
    플레이어 컨트롤러는 게임에서 플레이어의 입력과 출력되는 화면을 책임진다.

    현재 플레이 버튼을 눌러 콘텐츠를 테스트할 때는 매번 언리얼 에디터 뷰포트를 클릭해 포커스를
    잡아줘야 하지만 플레이어 컨트롤러에게 UI를 배제하고 게임에게만 입력을
    전달하도록 명령을 내리면 앞으로 편리하게 테스트가 가능하다.

*/