/*
    움직이는 액터를 제작하기 위해 일단 코딩의 흐름을 파악하고
    디버깅을 용이하게 만드는 로깅 환경을 구축해준다.

    출력 로그 윈도우

    로깅 환경을 위해 UE_LOG라는 매크로를 사용한다.

    UE_LOG(카테고리, 로깅 수준, 형식 문자열, 인자..)

    로그는 출력 로그(windows->output log) 혹은 프로젝트의 Saved 폴더 내에 있는
    Logs 폴더에서 확인 가능하다.

    로그 카테고리

    모든 로그에는 분류를 위한 카테고리가 지정된다.

    로깅 수준

    로그의 중요도를 나타내는 로깅 수준은 
    메시지(Log) - 흰색, 경고(Warning) - 노란색, 에러(Error) - 붉은색 세가지로 나뉜다.

    색상은 에디터 설정에서 변경 가능하다.

    로그 필터

    출력 로그 윈도우에는 필터 기능이 있어 원하는 로깅 수준과 카테고리의 정보만 출력 가능하다.

    형식 문자열

    로그 매크로는 다양한 데이터를 하나의 문자열로 조합해 출력하도록 C 언어의 prinf와 비슷한
    형식 문자열 기능을 지원한다.

    문자열 정의 시 모든 플랫폼에서 2바이트 문자열을 지원하는 TEXT 매크로를 사용하는 것이 좋다.
    UE는 문자열의 기본 클래스로 FString 클래스를 제공한다.
    FString으로 선언된 변수에서 문자열 정보를 얻어오려면 반드시 * 연산자를 앞에 지정해줘야 한다.

    ex) 액터의 이름 값을 FString으로 반환하는 GetName() 함수를 형식 문자열의 %s에 대응하는 경우,
        함수 앞에 * 연산자를 선언해준다.

        FString::Printf(TEXT("Actor Name : %s, ID : %d, Loaction X : %.3f"), *GetName(), ID, GetActorLocation().X);

        *는 UE만의 방식이다.

    로깅을 위한 공용 매크로 설정

    로그 카테고리를 직접 선언해 필요한 정보를 유용하게 분류할 수 있다.

    두 개의 매크로를 사용해 로그 카테고리를 정의한다.
    하나는 선언부, 하나는 구현부에 사용한다.
    게임 모듈명으로 된 헤더 파일과 소스 파일에 매크로를 각각 선언하는 것이 일반적이다.
    ex:
        ArenaBattle.h, ArenaBattle.cpp

        ArenaBattle이라는 로그 카테고리를 정의한다.

        헤더 파일에

        DECLARE_LOG_CATEGORY_EXTERN(ArenaBattle, Log, All);
        그리고 CorMinimal.h 대신 EngineMinimal.h를 include해준다.

        소스 파일에

        DEFINE_LOG_CATEGORY(ArenaBattle);

        이렇게 선언한 후 분수대 액터에서 EngineMinimal을 #include "ArenaBattle.h"로 변경해 불러온다.

    
    로깅
    UE_LOG(ArenaBattle, Warning, TEXT("Actor Name : %s, ID : %d, Loaction X : %.3f"), *GetName(), ID, GetActorLocation().X);

    추가 매크로 정의로 간편하게 로그를 설정한다.

    로그 카테고리를 고정하고 로그를 남길 떄 추가로 발생한 함수 이름과 코드 라인을 함께 출력하도록 매크로를 제작한다.
    두 종류의 로그 매크로를 ArenaBattle.h에 선언해준다.

    1. ABLOG_S : 
        코드가 들어있는 파일 일므과 함수, 그리고 라인 정보를 추가해 ArenaBattle 카테고리로 로그를 남긴다.
        로그를 사용한 함수의 실행 시점을 파악할 때 유용하다.

    2. ABLOG :
        ABLOG_S 저옵에 형식 문자열로 추가 정보를 지정해 로그를 남긴다.

    ex)
        #define ABLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
        #define ABLOG_S(Verbosity) UE_LOG(ArenaBattle, Verbosity, Text("%s"), *ABLOG_CALLINFO)
        #define ABLOG(Verbosity, Format, ...) UE_LOG(ArenaBattle, Verbosity, TEXT("%s %s", *ABLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

    이 매크로들을 활용해 로그를 남긴다.

    ex)
        ABLOG_S(Warning);
        ABLOG(Warning, TEXT("Actor Name : %s, ID : %d, Location : %.3f"), *GetName(), ID, GetActorLocation().X);
*/