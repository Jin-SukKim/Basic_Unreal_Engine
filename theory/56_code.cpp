/*
    무기 액터와 캐릭터의 AttackRange 값을 다르게 설정한다.
    이미 무기를 들고 있어도 무기를 변경할 수 있도록 해준다.

    ABWeapon.h
    ...
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	float AttackRange;
    ...
    float GetAttackRange() const;
    ...
    ABWeapon.cpp
    ... 생산자
    AttackRange =150.f;
    ...
    float AABWeapon::GetAttackRange() const
    {
        return AttackRange;
    }
    ...
    ABCharacter.h
    ...
    float GetFianlAttackRange() const;
    ...
    ABCharacter.cpp
    ...
    float AABCharactter::GetFianlAttackRange() const
    {
        return (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackRange() : AttackRange;
    }
    ..
    bool AABCharactter::CanSetWeapon()
    {
        return true;
    }
    void AABCharactter::SetWeapon(AABWeapon* NewWeapon)
    {
        ABCHECK(nullptr != NewWeapon);

        // 무기를 가지고 있다면 새 무기를 얻기위해 있는걸 없애준다.
        if (nullptr != CurrentWeapon)
        {
            CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            CurrentWeapon->Destroy();
            CurrentWeapon = nullptr;
        }
        ...
    }
    void AABCharactter::AttackCheck()
    {
        float FianlAttackRange = GetFianlAttackRange();
    ...
    #if ENABLE_DRAW_DEBUG
        // 탐색 시작에서 끝 지점까지 향하는 벡터
        FVector TraceVec = GetActorForwardVector() * FianlAttackRange;
        // 벡터의 중점 위치
        FVector Center = GetActorLocation() + TraceVec * 0.5f;
        // 벡터 길이의 절반
        float HalfHeight = FianlAttackRange * 0.5f + AttackRadius;
    ...
    }
    ...
    BTDecorator_IsInAttackRange.cpp
    ...
    bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
    {
        ...
        // AI가 컨트롤하는 캐릭터 가져오기
        auto ControllingPawn = Cast<AABCharactter>(OwnerComp.GetAIOwner()->GetPawn());
        if (nullptr == ControllingPawn)
            return false;
        ...
        // 목표와 AI의 거리가 충분히 가까운지 확인
        bResult = (Target->GetDistanceTo(ControllingPawn) <= ControllingPawn->GetFianlAttackRange());
        return bResult;
    }
    ...

    무기에 랜덤한 공격력을 부여해 추가 공격력과 효과치가 캐릭터 능력에 더해지게 해준다.
    ABWeapon.h
    ...
        UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite, Category = Attack)
	    float AttackDamageMin;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
        float AttackDamageMax;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
        float AttackModifierMin;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
        float AttackModifierMax;

        UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite, Category = Attack)
        float AttackDamage;

        UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite, Category = Attack)
        float AttackModifier;
    public:	
        ...
        float GetAttackDamage() const;
        float GetAttackModifier() const;
    ...
    ABWeapon.cpp
    ...
    // Sets default values
    AABWeapon::AABWeapon()
    {
        ...
        AttackDamageMin = -2.5f;
        AttackDamageMax = 10.f;
        AttackModifierMin = 0.85f;
        AttackModifierMax = 1.25f;
    }

    // Called when the game starts or when spawned
    void AABWeapon::BeginPlay()
    {
        Super::BeginPlay();

        AttackDamage = FMath::RandRange(AttackDamageMin, AttackDamageMax);
        AttackModifier = FMath::RandRange(AttackModifierMin, AttackModifierMax);
        ABLOG(Warning, TEXT("Weapon Damage : %f, Modifier : %f"), AttackDamage, AttackModifier);
        
    }
    float AABWeapon::GetAttackRange() const
    {
        return AttackRange;
    }

    float AABWeapon::GetAttackDamage() const
    {
        return AttackDamage;
    }

    float AABWeapon::GetAttackModifier() const
    {
        return AttackModifier;
    }
    ...
    ABCharacter.h
    ...
    float GetFianlAttackDamage() const;
    ...
    ABCharacter.cpp
    ...
    float AABCharactter::GetFianlAttackDamage() const
    {
        float AttackDamage = (nullptr != CurrentWeapon) ? (CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage())
            : CharacterStat->GetAttack();
        float AttackModifier = (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackModifier() : 1.f;
        return AttackDamage * AttackModifier;
    }
    ...
    void AABCharactter::AttackCheck()
    {
        ...
        if (bResult)
        {
            if (HitResult.GetActor()->IsValidLowLevel())
            {
                ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName());

                FDamageEvent DamageEvent;
                // 대미지 전달
                // 대미지는 폰에게 명령을 내리는 플레이어 컨트롤러이므로 컨트롤러 정보를 보내준다.
                // 레벨마다 주는 대미지가 바뀌므로 StatComponent에서 대미지 값을 가져온다.
                HitResult.GetActor()->TakeDamage(GetFianlAttackDamage(), DamageEvent, GetController(), this);
            }
        }
    }

    게임 스코어를 기반으로 NPC의 레벨을 조정해 게임 난이도를 조절한다.

    ABGameMode.h
    ...
    int32 GetScore() const;
    ...
    ABGameMode.cpp
    ...
    int32 AABGameMode::GetScore() const
    {
        return ABGameState->GetTotalGameScore();
    }
    ...
    ABCharacter.cpp
    ...
    void AABCharactter::SetCharacterState(ECharacterState NewState)
    {
        ...
        switch (CurrentState)
        {
        // LOADING state : 선택한 캐릭터 애셋을 로딩하는 state
        case ECharacterState::LOADING:
        {
            ...
            // NPC의 경우 스코어로 레벨 조정
            else
            {
                // GameMode에서 점수를 가져온다.
                auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
                ABCHECK(nullptr != ABGameMode);
                // 점수 * 0.8을 해 레벨 조정
                int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f));
                // 최소 1에서 최대 20 레벨
                int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
                ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
                CharacterStat->SetNewLevel(FinalLevel);
            }
            ...
            break;
        }
        ...
    }

*/