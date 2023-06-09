/*
    UPROPERTY 매크로를 사용해 자동으로 메모리를 관리할 수 있다.
    이 기능은 언리얼 오브젝트란 특별한 객체에만 사용 가능하다.

    언리얼 오브젝트는 언리얼 실행 환경에 의해 관리되는 C++ 객체다.
    콘텐츠를 구성하는 객체들은 모두 언리얼 오브젝트라고 볼 수 있따.

    C++ 클래스가 언리얼 오브젝트 클래스가 되기위해 언리얼 엔진이
    정의한 특별한 매크로와 규칙을 부여해야 한다

    1. 클래스 선언 매크로 :
        UCLASS() - 해당 클래스가 언리얼 오브젝트임을 선언하기 위한 매크로
        클래스 선언의 윗줄에 UCLASS 매크로를 선언하고 클래스 내부에
        GENERATED_BODY() 매크로를 선언한다.

    2. 클래스 이름 접두사 : 
        언리얼 오브젝트에는 항상 규칙에 맞게접두사가 붙어야 한다
        접두사는 크게 U와 A가 제공된다.

        A : 액터 클래스
        U : 액터가 아닌 클래스

        ex) 분수대는 액터이므로 AFountain이라는 클래스 이름을 가지고
            액터의 구성 요소인 스태틱메시 컴포넌트느 액터가 아니므로 UStaticMeshComponenet란 이름을 가진다.

    3. generated.h 헤더 파일 : 
        소스 코드를 컴파일하기 전 언리얼 엔진은 Unreal Header Tool 이란 도구를 사용해
        클래스선언을 분석하고 언리얼 실행 환경에 필요한 부가 정보를 별도의 파일에 생성한다.

        자동으로 생성되는 부가 파일이 generated.h 파일이다.
        컴파일 과정에 필연적으로 발생하기에 #include 마지막 구문에 이 헤더 파일을 반드시 선언한다.

        ex) include "Fountain.generated.h"

    4. 외부 모듈에의 공개 여부 :
        윈도우의 DLL 시스템은 DLL 내 클래스 정보를 외부에 공개할지 경정하는
        _declspec(dllsxport)라는 키워드를 제공한다.

        언리얼 엔진에서 이 키워드르 사용하려면 '모듈명_API'라는 키워드를 클래스 선언 앞에추가한다.
        이 키워드가 없으면 다른 모듈에서 해당 객체에 접근할 수 없다.

        ex) class ARENABATTLE_API AFountain : public AActor {}

    F 접두사 : 언리얼 오브젝트와 관련 없는 일반 C++ 클래스 혹은 구조체를 의미한다.


*/