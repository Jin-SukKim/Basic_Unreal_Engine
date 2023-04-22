/*
    객체 유형과 값 유형

    언리얼 오브젝트의 속성 값은 객체를 관리하는 객체 유형과 값을 관리하는 값 유형으로 나뉜다.
    (ex: 언리얼 오브젝트 클래스의 포인터는 객체 유형이다)

    대표적인 값 유형

    1. byte : uint8

    2. int : int32

    3. float : float

    4. string : FString, FName

    5. 구조체 : FVector, FRotator, FTransform

    이 값 유형으로 클래스 멤버 변수를 선언하고 UPROPERTY 매크로를 설정해주면
    매크로 선언만으로도 미리 예약된 기본값이 지정된다.

    ex) 정수 유형의 멤버 변수에 UPROPERTY 매크로를 선언해주면 반드시 0의 초기 값을 보장 받는다.

*/