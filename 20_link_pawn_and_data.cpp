/*
    폰과 데이터 연동

    애님 인스턴스에서 선언한 속성을 활용해 상황에 따라 다른 애니메이션을 제생하는 시스템을 제작했다.
    실제 게임에서 폰의 속도에 따라 다른 애니메이션을 재생하기 위한
    프레임마다 폰의 속력와 애님 인스턴스 변수 값이 같은 값을 가져야 한다.

    두 데이터를 동일하게 만들려면 폰의 Tick 함수에서 애님 인스턴스의 변수에 쓰거나
    애님 인스턴스의 Tick에서 폰의 속도 정보를 가져온 후 이를 변수에 업데이트하는 방법이 있다.

    후자의 방법에서 애님 인스턴스 클래스는 Tick마다 호출되는 NativeUpdateAnimation 함수를 제공한다.
    TryGetPawnOwner로 폰에 접근할 수 있다.

    auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
	}

    게임 엔진은 틱마다 입력 시스템 -> 게임 로직 -> 애니메이션 시스템 순으로 로직을 실행한다.
    애니메이션 시스템은 폰에 접근할 때 먼저 폰 객체가 유효한지 점검해야하는데
    검사하는 명령이 TryGetPawnOwner다.

    이 방법이 일반적이지만 폰에서 스켈레탈 메시 컴포넌트의 GetAnimInstance 함수를 사용하면
    폰에서 애님 인스턴스에 접근할 수 있다.

    AnimInstance.h
    void SetPawnSpeed(float NewPawnSpeed) { CurrentPawnSpeed = NewPawnSpeed; }

    AnimInstance.cpp
    void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
    {
        Super::NativeUpdateAnimation(DeltaSeconds);
    }

    Character.cpp
    #include "ABAnimInstance.h"
    ...
    auto ABAnimInstance = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
    if (nullptr != ABAnimInstance)
    {
        ABAnimInstance->SetPawnSpeed(GetVelocity().Size());
    }

    하지만 주로 AnimInstance 클래스에서 수행하는데 이러면 애니메이션 로직과 폰의 로직을 분리할 수 있어
    애니메이션이 필요 없는 서버 코드에도 문제없이 동작해서이다.
*/