/*
    콤보 공격의 구현

    각 공격 동작(애니메이션)을 섹션으로 분리한 후 일정 시간 내에
    공격 명령을 내리면 다음 공격 동작으로 이동하는 콤보 공격을 구현한다.

    각 공격 애니메이션마다 새로운 섹션을 추가하고 각 섹션이 하나의 공격 애니메이션이 할당되도록한다.
    우클릭 add section으로 추가해준다.

    그리고 우측 Montage Section 창을 보면 모든 애니메이션이 연동돼 있다.
    이 경우 첫 번째 섹션 몽타주를 재생하면 자동으로 모든 섹션의 애니메이션이 재생한다.

    수동으로 섹션을 하나씩 재생할 예정이므로 섹션 사이의 연동을 끊어 독립적으로 구동되도록한다.
    화살표 클릭 -> remove link

    노티파이 트랙을 하나 더 추가해준다.
    add notify track

    이 노티파이는 다음 공격으로 점프할 타이밍을 파악하는 노티파이다.
    이 노티파이는 공격 모션이 끝나고 다시 IDLE 모션으로 되돌아가기 전으로 설정한다.
    마지막 4번째는 더 이상 연계될 애니메이션 없으므로 설정하지 않는다.

    노티파이를 해당 프레임에 즉각적으로 반응하는 방식인 Branching Point 값으로
    Tick Type을 변경해준다.
    기본값인 Queued는 비동기 방식으로 신호를 받게 돼서 적절한 타이밍에
    신호를 받는것을 놓칠수도 있따.
    Queued는 주로 타이밍에 민감하지 않은 사운드나 이펙트를 발생시킬때 사용하는게 좋다.

    Character에 캐릭터가 사용할 수 있는 콤보 카운터의 최대치(MaxCombo),
    현재 실행 중인 콤보 카운터(CurrentCombo), 다음 콤보로의 이동 가능 여부(CanNextCombo),
    콤보 입력 여무(IsCombiInputOn)을 변수로 선언해준다.

    그리고 공격이 시작할 떄 관련 속성을 지정하도록 AttackStartComboState 함수와
    공격이 종료할 때 사용할 AttackEndComboState 함수를 선언한다.

    Character.h
    ...
    // 공격 시작 시점의 상태를 확인
	void AttackStartComboState();
	// 공격 종료 시점의 상태를 확인
	void AttackEndComboState();
    ...
    // 다음 단계 공격이 가능한지 확인하는 변수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	bool CanNextCombo;

	// 다음 단계 공격의 입력이 있는지 확인하는 변수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	bool IsComboInputOn;

	// 현재 콤보 수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	int32 CurrentCombo;

	// 최대 콤보 수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	int32 MaxCombo;

    Character.cpp
    생성자...
    MaxCombo = 4;
	AttackEndComboState();
    ...
    void AABCharactter::AttackStartComboState()
    {
        CanNextCombo = true;
        IsComboInputOn = false;
        ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
        CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
    }
        
    void AABCharactter::AttackEndComboState()
    {
        IsComboInputOn = false;
        CanNextCombo = false;
        CurrentCombo = 0;
    }

    ABAnimInstance 클래스에서 콤보 카운트를 전달받으면 해당 몽타주 섹션을 재생하는 기능을 구현한다.
    그리고 앞서 선언한 NextAttackCheck 애니메이션 노티파이가 발생할 때마다
    ABChacacter에 이를 전달한 델리게이트를 선언하고 애니메이션 노티파이 함수에서 이를 호출한다.

    델리게이트 기능을 사용하면, 애님 인스턴스는 자신의 델리게이트를 사용하는 객체가 어떤 것인지
    몰라도 델리게이트에 연결된 함수만 호출하면 되므로 다른 클래스와 연결되지 않는 의존성 없는 설계를 할 수 있따.

    반환 값과 인자 값이 없는 함수 유형으로 델리게이트를 선언하되, 여러 개의 함수가 등록되도록
    멀티캐스트로 선언한다.
    멀티캐스트 델리게이트에 등록된 모든 함수를 호출한다.
    Broadcast로 설정한다.

    AnimInstance.h
    클래스밖..
    // 함수 여러개를 한번에 호출 가능한 멀티캐스트 델리게이트 선언
    DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
    DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
    ...
    클래스안
    ...
    // 다음 몽타주 색션을 실행시킬 함수
	// GetAttackMontageSectionName과 함께 사용해 몽타주 이름을 받는다.(Attack1, Attack2 등)
	void JumpToAttackMontageSection(int32 NewSection);
    ...
	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	FName GetAttackMontageSectionname(int32 Section);

    AnimInstance.cpp
    ...
    void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
    {
        ABCHECK(Montage_IsPlaying(AttackMontage));
        Montage_JumpToSection(GetAttackMontageSectionname(NewSection), AttackMontage);
    }

    void UABAnimInstance::AnimNotify_AttackHitCheck()
    {
        OnAttackHitCheck.Broadcast();
    }

    void UABAnimInstance::AnimNotify_NextAttackCheck()
    {
        OnNextAttackCheck.Broadcast();
    }

    FName UABAnimInstance::GetAttackMontageSectionname(int32 Section)
    {
        ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
        return FName(*FString::Printf(TEXT("Attack%d"), Section));
    }
    ...

    Character.cpp
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
    }
    
    void AABCharactter::Attack()
    {
        if (IsAttacking)
        {
            ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
            if (CanNextCombo)
            {
                IsComboInputOn = true;
            }
        }
        else
        {
            ABCHECK(CurrentCombo == 0);
            AttackStartComboState();
            ABAnim->PlayAttackMontage();
            ABAnim->JumpToAttackMontageSection(CurrentCombo);
            IsAttacking = true;
        }
    }
    
    void AABCharactter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
    {
        ABCHECK(IsAttacking);
        ABCHECK(CurrentCombo > 0);
        IsAttacking = false;
        AttackEndComboState();
    }


*/