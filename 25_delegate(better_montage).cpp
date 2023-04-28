/*
    델리게이트

    이전에는 몽타주 시스템이 구동 중이면 플레이되지 않도록 기능을 구현했지만
    계속 구동 중인지를 체크하는 방식보다 몽타주 재생이 끝나면 공격이 가능하다고
    폰에게 알려주는 방식이 더 효과적이다.

    Delegate 기능르 사용하면 후자의 방식으로 구현할 수 있다.

    델리게이트는 넓은 의미로 특정 객체가 해야 할 로직을 다른 객체가
    대신 처리할 수 있도록 만드는 보편적인 설계의 개념이다.

    UE의 델리게이트는 A 객체가 B 객체에 작업 명령을 내릴 때 B 객체에
    자신을 등록하고 B의 작업이 끝나면 A에게 알려주는 설계 방식을 의미한다.

    A가 B가 요구하는 형식으로 멤버 함수를 만들면 이를 B에 등록할 수 있고,
    B가 특정 상황이 될 때는 B는 미리 등록해둔 A의 멤버 함수를 호출해주는 방식으로 동작한다.

    1. A의 함수를 B에 등록
    2. B에게 명령
    3. B가 명령을 실행
    4. 등록한 A의 함수 호출

    이를 위해 델리게이트 프레임워크를 사용해야 한다.
    애님 인스턴스에는 몽타주 재생이 끝나면 발동하는 OnMontageEnded라는 델리게이트를 제공한다.

    어떤 언리얼 오브젝트라도 UAnimMontage* 인자와 bool 인자를 가진 멤버 함수를 가지고 있다면,
    이를 OnMontageEnded 델리게이트에 등록해 몽타주 재생이 끝나는 타이밍을 파악할 수 있다.

    Character 액터에 위 함수 형식을 선언하고 윗줄에 UFUNCTION 매크로를 추가로 선언한다.
    OnMontageEnded 델리게이트는 블루프린트의 함수와도 연동할 수 있도록 설계돼
    C++에서 연동하려는 함수는 블루프린트와 호환되는 함수형으로 선언해야 한다.

    UE에서 델리게이트 종류

    1. C++ 객체에만 사용가능한 델리게이트
    2. 블루프린트/C++ 객체 모두 사용할 수 있는 델리게이트

    액터가 생성되는 PostInitializeComponents에 선언한 함수를 애님 인스턴스의
    OnMontageEnded 델리게이트에 바인딩한다.
    현재 공격 중인지 아닌지를 파악할 수 있도록 bool 변수를 선언해준다.

    Character.h
    ...
    virtual void PostInitializeComponents() override;
    ...
    UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(VisibleIntanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	bool IsAttacking;

    // 애님 인스턴스
	UPROPERTY()
	class UABAnimInstance* ABAnim;
    ...

    이번에는 UE가 제공하는 check나 ensure 어설션대신 ABCHEKC 매크로를 추가해
    런타임에서 문제 발생 시 에러 로그를 띄우고 함수를 반환하도록 한다.

    ArenaBattle.h
    ...
    #define ABCHECK(Expr, ...) { if (!(Expr)) {ABLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; }}
    ...

    매크로를 활용해 애님 인스턴스의 OnMontageEnded 델리게이트와 선언한 OnAttackMontageEnded를 연결해,
    델리게이트가 발동할 때까지 애니메이션 시스템에 몽타주 재생 명령을 내리지 못하게 폰 로직에서 막아준다.

    ArenaBattle.cpp
    생성자 ...
    IsAttacking = false;
    ...
    void AABCharactter::PostInitializeComponents()
    {
        Super::PostInitializeComponents();

        ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
        // 애님 인스턴스의 OnMontageEnded 델리게이트와 선언한 OnAttackMontageEnded를 연결해,
        // 델리게이트가 발동할 때까지 애니메이션 시스템에 몽타주 재생 명령을 내리지 못하게 폰 로직에서 막아준다.
        ABCHECK(nullptr != ABAnim);

        ABAnim->OnMontageEnded.AddDynamic(this, &AABCharactter::OnAttackMontageEnded);
    }
    ...
    void AABCharactter::Attack()
    {
        if (IsAttacking) return;

        // PostInitializeComponent에서 AnimInstance 변수를 가져온다.
        //auto AnimInstance = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
        //if (nullptr == AnimInstance) return;

        ABAnim->PlayAttackMontage();
        IsAttacking = true;
    }

    void AABCharactter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
    {
        ABCHECK(IsAttacking);
        IsAttacking = false;
    }

    델리게이트로 공격의 시작과 종료가 감지되므로 AnimInstance에선 그냥 몽타주가
    재생되는지 확인하지 않고 플레이 해주면 된다.

*/