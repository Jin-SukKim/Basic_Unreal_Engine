/*
    모듈과 빌드 설정

    UE의 소스는 많은 수의 모듈이 뭉쳐진 집합으로 구성돼 있는데, 보통 언리얼 에디터가
    자동으로 생성하는 개발 환경에 의해 핵심적인 모듈만 개발 환경에 지정해 사용한다.
    (...Build.cs 파일에서 확인 가능하다)

    보통 Core, CoreUObject, Engine, InputCore란 모듈 네개를 사용한다.

    위젯 컴포넌트 기능은 UMG라는 모듈을 추가해줘야 사용가능하다.
    만약 EnhancedInput을 사용하려면 EnhancedInput 모듈을 추가해줘야 한다.

    PublicDependencyModuleNames.AddRange 함수에 UMG 모듈을 추가해준다.
    그 후 컴파일을 해주면 모듈이 언리얼 C++ 프로젝틀 관리를 담당하는 툴인
    Unreal Build Tool이 모듈의 Build.cs 파일을 분석해 프로젝트 컴파일 설정을 조정한다.

    모듈을 더해주면 헤더 파일의 경로도 별도의 설정 없이 바로 사용할 수 있도록 기본 경로로 설정된다.
    (#include "Components/InputComponent.h", #include "Compoennts/WidgetComponent.h" 등)

    프로젝트 내 Source -> Runtime 에서 찾으면 모듈 이름의 폴더가 존재한다.
    모듈 추가시 전체 경로를 몰라도 자유롭게 참조가 가능해진다.
*/