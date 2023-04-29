/*
    디버그 드로잉

    공격할 때마다 로그 창을 열고 매번 탐지하는 것은 번거롭다.
    공격 범위가 시각적으로 보이지 않다보니, 공격 범위, 탐지, 맞았는지 아닌지를 파악할 수 없다.
    이럴 떄 UE에서 제공하는 Debug Drawing 기능을 사용한다.

    이 기능을 사용하기 위해 DrawDebugHelpers.h를 추가한다.
    이 헤더에는 다양한 그리는 함수들이 선언돼 있는데 그 중 DrawDebugCapsule 함술를
    사용해 탐색을 위해 원이 움직인 궤적을 표현한다.

    캡슐의 반지름은 50, 탐색 시작 위치에서 탐색 끝 위치로 향하는 벡터를 구한 후,
    벡터의 중점 위치와 벡터 길이의 절반을 대입해 원하는 크기의 캡슐 모양을 구한다.

    캡슐은 상하로 서있는 모습이므로 회전 행렬을 적용해 캐릭터 시선 방향으로 눕힌 후
    공격 범위에 맞게 길이를 설정한다.

    ABCharacter.h
    ...
    // DefaultsOnly 는 블루프린트 편집화면에서만 보여지고
	// InstanceOnly 는 뷰포트에서만 보여진다
	// 공격 범위 길이
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	// 공격 범위 반지름
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;
    ...

    ABCharacter.cpp
    .. 생성자
    // 공격 범위 설정
	AttackRange = 200.f;
	AttackRadius = 50.f;
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
            GetActorLocation() + GetActorForwardVector() * AttackRange, // 도형의 탐색 영역의 끝 지점은 액터 시선 방향으로 200cm 떨어진곳
            FQuat::Identity,    // 회전값은 기본값을 지정한다.
            ECollisionChannel::ECC_GameTraceChannel2, // 물리 충돌 감지에 사용할 트레이스 채널 정보(Attack 채널)
            FCollisionShape::MakeSphere(AttackRadius), // 탐지에 사용할 도형을 제작(50cm 반지름의 구체)
            Params  // 탐색 반응 설정으로 구조체의 기본값을 사용(탐색 방법)
        );

    #if ENABLE_DRAW_DEBUG

        // 탐색 시작에서 끝 지점까지 향하는 벡터
        FVector TraceVec = GetActorForwardVector() * AttackRange;
        // 벡터의 중점 위치
        FVector Center = GetActorLocation() + TraceVec * 0.5f;
        float HalfHeight = AttackRange * 0.5f + AttackRadius;
        // 캡슐을 캐릭터 시선 방향으로 눕혀 공격범위에 맞게 설정해준다.
        FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
        // 공격에 성공하면 초록색 실패하면 빨간색으로 설정
        FColor DrawColor = bResult ? FColor::Green : FColor::Red;
        float DebugLifeTime = 5.f;

        // 캡슐모양 그려주기
        DrawDebugCapsule(
            GetWorld(),
            Center,
            HalfHeight,
            AttackRadius,
            CapsuleRot,
            DrawColor,
            false,
            DebugLifeTime
        );


    #endif

        if (bResult)
        {
            if (HitResult.GetActor()->IsValidLowLevel())
            {
                ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName());
            }
        }
    }
*/