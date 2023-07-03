/*
    대미지 프레임워크

    여태 물리 엔진을 사용해 공격 영역을 설정하고 그 공격을 받을 액터를 감지했다.
    이제 공격 행동을 완성하려면 감지된 액터에 대미지를 전달해야한다.

    UE이 제공하는 대미지 프레임워크를 사용하면 대미지에 관련된 여러 기능을 간편하게 처리할 수 있다.

    UE의 액터 클래스 AActor는 TakeDamage라는 함수가 구현돼 있다.
    이 함수를 사용하면 손쉽게 액터에 대미지를 전달할 수 있다.
    TakeDamage 함수는 총 4개의 인자를 가지고 있으며 각 인자의 용도는 다음과 같다.

    1. DamageAmount : 전달할 대미지의 세기
    2. DamageEvent : 대미지 종류
    3. EventInstigator : 공격 명령을 내린 가해자
    4. DamageCauser : 대미지 전달을 위해 사용한 도구

    대미지를 전달하는 행위에는 항상 가해자와 피해자가 존재한다.
    가해자란 피해를 입히는 주체를 의미하는데, 여기서 대미지를 가한 진정한 가해자는
    폰이 아닌 폰에게 명령을 내린 플레이어 컨트롤러라고 할 수 있다.
    따라서 EventInstigator에는 폰이 아닌 컨트롤러 정보를 보내줘야 한다.

    공격 범위 내에 감지된 액터에 대미지를 전달하는 로직에서 폰은 플레이어가 대미지 전달을
    위해 사용하는 도구라고도 해석할 수 있기 때문에 마지막 파라미터에 지정한다.

    ABCharacter.cpp
    ...
    void AABCharacter::AttackCheck(){
        ...
        if (bResult)
        {
            if (HitResult.GetActor()->IsValidLowLevel())
            {
                ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName());

                FDamageEvent DamageEvent;
                // 대미지는 폰에게 명령을 내리는 플레이어 컨트롤러이므로 컨트롤러 정보를 보내준다.
                HitResult.GetActor()->TakeDamage(50.f, DamageEvent, GetController(), this);
            }
        }
    }

    대미지는 잘 전달됐고 이제 피해를 입은 앱터에 관련 로직을 구성해줘야 최종 완성된다.
    액터의 TakeDamage 함수를 override해 액터가 받은 대미지를 처리하는 로직을 추가한다.
    TakeDamage 함수는 부모 클래스인 AActor에 기본적인 대미지 관련 로직이 구현돼 있기에
    Super 키워드르 사용해 부모 클래스의 로직을 먼저 실행시켜 준다.

    ABCharacter.h
    ...
    // 받은 대미지 처리하는 함수
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
    ...
    ABCharacter.cpp
    ...
    float AABCharactter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
    {
        float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
        ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
        return FinalDamage;
    }
    ...

    모든 액터의 설정에는 Can be Damaged 속성이 있어 이 속성의 체크를 해제하면
    캐릭터에 전달된 대미지의 결과가 모두 0이 되는 무적 상태로 변한다.

    대미지를 받으면 캐릭터가 사망하는 애니메이션을 설정해본다.
    죽는 애니메이션 재생을 위해 ABAnimInstance에서 IsDead 이름의 속성을 추가한다.

    ABAnimInstance.h
    ...
    void SetDeadAnim() {
		IsDead = true;
	}
    ...
    // 죽는 애니메이션을 위한 bool
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
    ...
    ABAnimInstance.cpp
    ... 생성자
    IsDead = false;
    ...
    void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
    {
        Super::NativeUpdateAnimation(DeltaSeconds);

        // 폰에 접근해 폰의 속렵 값은 얻어온다.
        auto Pawn = TryGetPawnOwner();
        if (!::IsValid(Pawn)) return;

        if (!IsDead)
        {
            CurrentPawnSpeed = Pawn->GetVelocity().Size();
            auto Character = Cast<ACharacter>(Pawn);
            if (Character)
            {
                IsInAir = Character->GetMovementComponent()->IsFalling();
            }
        }
    }

    IsDead 속성을 추가하고 애니메이션 블루프린트로 이동해 애님 그래프를 수정해준다.
    죽는 애니메이션 재생 노드는 반복 재생하지 않도록 설정해준다.

    IsDead가 true면 죽는 애니메이션이 실행되도록 blend node by bool 노드를 생성해
    만들어준다.

    애니메이션 블루프린트 설정이 완료되면 죽은 후에는 SetActorEnableCollision 함수를 사용해
    액터의 충돌 설정을 꺼 앞으로 캐릭터에는 충돌 이벤트가 발생하지 않도록 한다.

    ABCharacter.cpp
    ...
    float AABCharactter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
    {
        float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
        ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

        if (FinalDamage > 0.f)
        {
            ABAnim->SetDeadAnim();
            SetActorEnableCollision(false);
        }
        return FinalDamage;
    }
*/