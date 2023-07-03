/*
    애셋의 지정

    C++ 코드에서 애셋이 자동으로 로딩되도록 기능을 추가한다.

    모든 애셋은 레벨 애셋을 제외하고 모두 uasset이라는 동일한 확장자를 가진다.
    이 애셋들은 외형적으로는 동일한 체계에서 관리되지만  내부적으로는 서로 다른 데이터를 가지는데,
    에셋 파일이 내부에 어떤 데이터를 가지고 있는지는 마우스를 올려보거나 에셋 아이콘 하단의 색상으로 파악 가능하다.

    스태틱메시 애셋을 확인해보면 StaticMesh.h란 언리얼 오브젝트의 데이터가 담겨져 있는걸 확인할 수 있다.

    스태틱메시 애셋을 불러들여 컴포넌트에 설정하는 기능

    C++ 코드에서 해당 애셋을 불러들이려면 애셋의 고유한 키 값을 파악하고
    애셋을 관리하는 시스템에서 애셋의 포인터를 가져와야 한다.
    UE는 애셋의 키 값으로 경로 값을 사용하며, 애셋 위에 마우스를 올려 확인 가능하다.

    하지만 따라 치는 건 번거롭기에 우클립 후 copy reference나 Ctrl+C 를 통해 경로 정보를 복사해준다.

    C++에서 붙여넣기를 하면 경로만 복사되는게 아니다.

    /Script/Engine.StaticMesh'/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01'

    이 정보는 복잡한 규칙을 가지고 있따.

    {오브젝트 타입}'{폴더명}/{파일명}.{애셋명}'

    1. 오브젝트 타입 : 애셋의 타입을 명시적으로 지정
    2. 폴더명/파일명 : 물리적인 디스크에 위치한 애셋의 경로 정보를 의미(중복될 수 없고 유일해야 한다.)
    3. 애셋명 : 에디터에서 보여지는 애셋의 이름(중복 가능)

    애셋 불러오는 기능

    1. ConstructorHelpers 클래스의 FObjectFinder를 사용해 변수를 선언해 경로 값을 전달한다.
    2. 변수의 Object 멤버 값을 사용해 애셋에 대한 포인터를 가져온다.
        (ex: 스태틱메시 애셋의 포인터를 가져오면 스태틱메시 컴포넌트의 SetStaticMesh 함수에 전달하면
             C++ 코드로 애셋을 로딩하는 기능이 완성된다.)

        if (SM_BODY.Succeeded())
        {
            Body->SetStaticMesh(SM_BODY.Object);
        }

    애셋의 경로 정보는 변경될 일이 없기에 static 선언으로 불필요한 호출을 방지해준다.


    ** 
    // static 붙이니깐 애셋 지정이 안된다.
	// 스태틱 매쉬의 나나이트와 텍스처의 버추얼 텍스처로 변환만 했는데 static으로 설정하면 안되는 이유는?
*/