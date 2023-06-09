/*
    어설션(Assertion)은 반드시 호강니하고 넘어가야하는 점검 코드를 의미한다.
    코드가 차후 더욱 복잡한 문제가 발생하지 않도록 실행은 중지하고 오류를 띄우는 것이
    탄탄한 개발을 위한 것이므로 어설션 구문을 사용한다.
    ex) 
        분수대 액터의 초기화에 지정한 스태틱메시 에셋이 경로에 존재하지 않으면
        게임 진행에 치명적인 오류를 발생시킬수 있는 경우 등

    UE는 다양한 어설션 구문을 제공한다.
    
    check 구문이 있는데 check 안에 조건을 통과하지 못하면 crash 화면이 뜨고 언리얼 에디터는 종료된다.

    어셜션 기능이 효과적으로 발휘되려면 디버깅 정보를 파악할 수 있는 디버깅 기호가
    엔진에 있어야 하는데, 엔진의 초기 옵션에는 디버깅 기호가 없다.

    그래서 에픽게임즈 런처에서 엔진 옵션을 선택해 디버깅 기호를 설치해준다.
    (디버깅을 위한 편집기 기호)

    중대한 문제에 check 어설션을 사용해 확실하게 경고하는 것은 좋은 방법일 수 있지만,
    에디터를 다시 종료하고 띄우는 시간이 만만치 않으므로 가볍게 경보만 내리는 것이 좋을때도 있다.

    이떄 사용하는 어설션으로 ensure를 주로 사용한다.

    이외에도 다양한 어설션이 존재한다.

*/