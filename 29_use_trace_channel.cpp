/*
    트레이스 채널의 활용

    트레이스 채널 : 어떤 행동에 설정하는 콜리전 채널
        - Visibility, Camera

    Visibility : 배경 물체가 시각적으로 보이는지 탐지하는 데 사용하는 채널
        탐지에서 폰은 제외된다.
        마우스로 물체르 선택하는 피킹(Picking) 기능을 구현할 때 사용한다.

    Camera : 카메라 설정을 위해 카메라와 목표물 간에 장애물이 있는지 탐지하는 데 사용하는 채널
        3인칭 카메라에서 캐릭터 조작 시 장애물이 시야를 가리면 카메라를 줌인하는 기능 등

    새로운 콜리전 채널로 캡슐 컴포넌트를 설정했다면, 물리 엔진을 사용해 캐릭터의 공격 기능을 구현해본다.

    게임에서 공격이라는 행동은 공격 애니메이션이 일어나는 특정 타이밍에 공격 범위 안에
    위치한 액터가 있는지 감지하고 감지된 액터에게 데미지를 전달하는 행위다.

    어떠한 행동에 대한 물리적 판정이 필요할 때 UE의 물리 엔진을 활용한다.
    UE은 행동에 대한 판정을 위해 트레이스 채널이라는 카테고리로 콜리전 채널을 제공한다.

    캐릭터의 공격판정을 위한 트레이스 채널을 하나 추가한다.
    Attack이란 이름으로 설정하고 기본 반응을 ignore로 설정한다.

    Preset 설정으로 이동해 앞서 제작한 콜리전 프리셋 ABCharacter를 연다.
    방금 추가한 Attack 트레이스 채널과 블록으로 지정한다.

    Attack은 기본 반응으로 Ignore이므로 Attack 트레이스 채널을 사용하는 액터의
    물리적 행동은 캐릭터의 캡슐 컴포넌트에 설정한 ABChrater 콜리전 프릿세에만 반응한다.

    트레이스 채널의 설정을 완료했으면 이제 공격 판정을 내리는 로직을 추가한다.
    트레이스 채널을 사용해 물리적 충돌 여부를 가리는 함수 중 하나로 SweepSingleByChannel이 있따.
    물리는 월드의 기능이므로 GetWorld() 함수를 사용해 월드에 명령을 내린다.

    SweepSingleByChannel 함수는 기본 도형을 인자로 받은 후 시작 지점에서 끝 지점까지 쓸면서(Sweep)
    해당 영역 내에 물리 판정이 일어났는지 조사한다.
    (여러개를 검출하고 있다면 sweepMultiByChannel을 사용한다)

    이 함수의 parameter 요소들

    1. HitResult : 물리적 충돌이 탐지된 경우 관련된 정보를 담을 구조체
    2. Start : 탐색을 시작할 위치
    3. End : 탐색을 끝낼 위치
    4. Rot : 탐색에 사용할 도형의 회전
    5. TraceChannel : 물리 충돌 감지에 사용할 트레이스 채널 정보
    6. CollisionShape : 탐색에 사용할 기본 도형 정보. 구체, 캡슐, 박스를 사용한다.
    7. Params : 탐색 방법에 대한 설정 값을 모아둔 구조체
    8. ResponseParams : 탐색 반응을 설정하기 위한 구조체

    공격의 범위를 설정하기 위해 반지름이 50cm인 구를 만들고 이를 캐릭터 위치에서 정면 방향으로
    2미터(200cm) 떨어진 곳까지 쓸어서 충돌하는 물체가 있는지 감지해본다.

    TraceChannel은 Attack 채널을 쓰고 채널의 값은 UE에서 정의한 ECollisionChannel 열거형으로
    가져올수 있는데 C++ 코드에서 가져오는 방법이 복잡하다.

    UE은 게임에서 활용할 수 있도록 총 32개으 ㅣ콜리전 채널을 제공한다.
    32개중 8개는 UE이 기본 콜리전 채널으로 사용하고, 6개는 엔진에서 다른 용도로 사용하도록 예약돼 있다.
    우리는 이를 뺀 나머지 18개만 사용할 수 있다.

    인젠 소스에서 ECollisionChannel 열겨형 선언 중 일부
    EngineTypes.h
    ...
    enum ECollisionChannel {
        ...
        ECC_GameTraceChannel1 UMETA(Hidden),
        ECC_GameTraceChannel2 UMETA(Hidden),
        ...
    }

    게임 프로젝트에서 새로 생성하는 오브젝트 채널과 트레이스 채널은 ECC_GameTraceChannel 1번부터
    18번까지 중에서 하나를 배정받는다.
    어떤 값을 배정받았는지는 프로젝트의 Config 폴더에 있는 DefaultEngine.ini에서 확인 가능하다.

    DefualtEngine.ini에 있는 콜리전 채널의 설정값은 다음과 같다.
    INI 파일 하단을 자세히 보면 생성한 Attack 틀레이스 채널은 ECC_GameTraceChannel2 열거형 값을 사용하고있다.

    DefualtEngine.ini
    ...
    [/Script/Engine.CollisionProfile]
    ...
    +DefaultChannelResponses=(Channel=ECC_GameTraceChannel1,DefaultResponse=ECR_Block,bTraceType=False,bStaticObject=False,Name="ABCharacter")
    +DefaultChannelResponses=(Channel=ECC_GameTraceChannel2,DefaultResponse=ECR_Ignore,bTraceType=True,bStaticObject=False,Name="Attack")
    ...

    콜리전 채널을 지정하면, FCollisionShape::MakeSphere 함수를 사용해 탐지에 사용할 도형을 제작한다.
    탐색할 도형으로는 50cm 반지름을 가지는 구체를 사용하고 회전값은 기본값을 지정한다.
    도형의 탐색 영역은 탐색을 시작할 위치는 액터가 있는 곳으로, 끝낼 위치는 액터 시선 방향으로 200cm 떨어진 곳으로 정한다.

    탐색할 영역을 지정했다면, 탐색 방법을 설정한다.
    공격 명령을 내리는 자신은 탐색에 감지되지 않도록 포인터 this를 무시할 액터 목록에 넣어준다.
    마지막 인자인 탐색 반응 설정은 구조체의 기본값을 사용한다.

    마지막으로 액터의 충돌이 탐지된 경우 충돌된 액터에 관련된 정보를 얻기 위해
    구조체를 넘겨주는데, FHitResult 구조체로 지역 변수를 하나 생성하고,
    이를 첫번째 인자에 넣어주면 탐색 기능이 완성된다.

    ABCharacter.h
    ...
    // 공격을 체크해주는 함수
	void AttackCheck();
    ..
    ABCharacter.cpp
    ...
    void AABCharactter::PostInitializeComponents()
    {
        Super::PostInitializeComponents();
        
        ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
        // 애님 인스턴스의 OnMontageEnded 델리게이트와 선언한 OnAttackMontageEnded를 연결해,
        // 델리게이트가 발동할 때까지 애니메이션 시스템에 몽타주 재생 명령을 내리지 못하게 폰 로직에서 막아준다.
        ABCHECK(nullptr != ABAnim);

        ABAnim->OnMontageEnded.AddDynamic(this, &AABCharactter::OnAttackMontageEnded);

        // NextAttackCheck 델리게이트에 로직을 선언하고 구현
        // 콤보가 가능한지 파악하고 상황에 따라 대응
        ABAnim->OnNextAttackCheck.AddLambda([this]() -> void {
            ABLOG(Warning, TEXT("OnNextAttackCheck"));
            CanNextCombo = false;

            if (IsComboInputOn)
            {
                AttackStartComboState();
                ABAnim->JumpToAttackMontageSection(CurrentCombo);
            }
        });

        ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharactter::AttackCheck);
    }
    ...
    void AABCharactter::AttackCheck()
    {
        FHitResult HitResult;
        // 탐색 방법으로 공격 명령을 내이는 자신은 탐색에 감지되지 않도록 포인터 this를 넣고
        // 탐색 반응 설정은 구조체의 기본값을 사용한다.
        // 인자 : TraceTag의 이름(NAME_None = 이름이 없다고 명시),
        // false = 복잡한 검사가 아닌 간단한 검사를 싱행
        // (true = complex collision test, 복잡한 검사로
        // 검출되는 첫번째 오브젝트, 충돌 지점 등 상세한 정보를 얻게된다.)
        // this = 충돌검사를 하는 객체이며 검사시에 제외된다.
        FCollisionQueryParams Params(NAME_None, false, this);
        bool bResult = GetWorld()->SweepSingleByChannel(
            HitResult,  // 액터의 충돌이 탐지된 경우 출돌된 액터에 관련된 정보를 얻기위한 구조체
            GetActorLocation(), // 도형의 탐색 영역의 시작 지점은 액터가 있는 곳
            GetActorLocation() + GetActorForwardVector() * 200.f, // 도형의 탐색 영역의 끝 지점은 액터 시선 방향으로 200cm 떨어진곳
            FQuat::Identity,    // 회전값은 기본값을 지정한다.
            ECollisionChannel::ECC_GameTraceChannel2, // 물리 충돌 감지에 사용할 트레이스 채널 정보(Attack 채널)
            FCollisionShape::MakeSphere(50.f), // 탐지에 사용할 도형을 제작(50cm 반지름의 구체)
            Params  // 탐색 반응 설정으로 구조체의 기본값을 사용(탐색 방법)
        );
        if (bResult)
        {
            if (HitResult.GetActor()->IsValidLowLevel())
            {
                ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName());
            }
        }
    }
    ...


*/