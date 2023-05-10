/*
    캐릭터 state machine의 열거형
    ArenaBattle.h
    ...
    // Blueprint에서도 사용 가능하게 UENUM을 선언해준다.
    UENUM(BlueprintType)
    enum class ECharacterState : uint8
    {
        PREINT,
        LOADING,
        READY,
        DEAD
    };
    ...

    state machine enum 타입의 변수를 선언하고 state machine 제작을 위한 기본 로직과 함수를 선언
    ABcharacter.h
    ...
    // 캐릭터 state machine
	void SetCharacterState(ECharacterState NewState);
	ECharacterState GetCharacterState() const;
    ...
    // 캐릭터 state machine
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	ECharacterState CurrentState;

	// 플레이어인지 AI인지 구분을 위한 변수
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool bIsPlayer;

	UPROPERTY()
	class AABAIController* ABAIController;

	UPROPERTY()
	class AABPlayerController* ABPlayerController;
    ...

    캐릭터는 PREINT state에서 시작
    게임 시작 시(BeginPlay 함수) 플레이어인지 NPC인지 파악 후 선택된 애셋을 로딩을 시작하며 state를 LOADING으로 변경
    애셋은 플레이어 일시 애셋 목록 중 4번째, NPC인 경우 랜덤으로 결정
    로딩 완료시 READY state로 넘어가며 게임 중 HP가 0 이하일 시 DEAD state로 변경

    ABCharacter.cpp
    ...생성자
    // 플레이어인 경우 캐릭터 애셋 목록 중 4번째 애셋을 사용한다.
	AssetIndex = 4;

	// PREINIT state에선 기본 캐릭터 애셋이 설정돼 있지만 캐릭터와 UI를 숨겨두고 대미지를 입지 않는다.
	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	SetCanBeDamaged(false);
    ...
    ABCharacter.cpp
    ...생산자
    // 플레이어인 경우 캐릭터 애셋 목록 중 4번째 애셋을 사용한다.
	AssetIndex = 4;

	// PREINIT state에선 기본 캐릭터 애셋이 설정돼 있지만 캐릭터와 UI를 숨겨두고 대미지를 입지 않는다.
	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	SetCanBeDamaged(false);
    ...
    // Called when the game starts or when spawned
    void AABCharactter::BeginPlay()
    {
        Super::BeginPlay();
        
        bIsPlayer = IsPlayerControlled();

        // 플레이어인지 NPC인지 확인
        if (bIsPlayer)
        {
            // 컨트롤러 가져오기
            if (ABPlayerController = Cast<AABPlayerController>(GetController()))
            {
                ABCHECK(nullptr != ABPlayerController);
                // enhancde input의 서브시스템
                if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ABPlayerController->GetLocalPlayer()))
                {
                    Subsystem->AddMappingContext(InputMapping, 0);
                }
            }
        }
        else
        {
            ABAIController = Cast<AABAIController>(GetController());
            ABCHECK(nullptr != ABAIController);
        }

        // ArenaBattleSetting에서 INI 파일의 애셋 경로를 읽어온다.
        auto DefaultSetting = GetDefault<UABCharacterSetting>();

        if (bIsPlayer)
        {
            AssetIndex = 4;
        }
        else
        {
            // 애셋 목록에서 경로들 중 하나 랜덤하게 가져오기
            AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
        }

        // Load할 캐릭터 애셋
        CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
        auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
        ABCHECK(nullptr != ABGameInstance);
        // AsyncLoad를 통해 비동기 방식으로 애셋을 로딩
        // 애셋이 로딩을 완료하면 해당 Delegate에 연결된 함수를 호출해준다.
        // Delegate에서 제공하는 CreateUObject 명령을 사용해 즉석에서 Delegate를 생성해 함수와 연동시켜준다.
        AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad,
            FStreamableDelegate::CreateUObject(this, &AABCharactter::OnAssetLoadCompleted));
        // 선택한 캐릭터 애셋을 로딩하는 state로 변경
        SetCharacterState(ECharacterState::LOADING);
    }
    void AABCharactter::SetCharacterState(ECharacterState NewState)
    {
        ABCHECK(CurrentState != NewState);
        CurrentState = NewState;
        
        switch (CurrentState)
        {
        // LOADING state : 선택한 캐릭터 애셋을 로딩하는 state
        case ECharacterState::LOADING:
        {
            SetActorHiddenInGame(true);
            HPBarWidget->SetHiddenInGame(true);
            SetCanBeDamaged(false);
            break;
        }
        // READY state : 캐릭터 애셋 로딩이 완료된 state
        case ECharacterState::READY:
        {
            SetActorHiddenInGame(false);
            HPBarWidget->SetHiddenInGame(false);
            SetCanBeDamaged(true);

            // HP가 0 이하면 DEAD state로 변경
            CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
                SetCharacterState(ECharacterState::DEAD);
                }
            );

            // 위젯의 초기화
            auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
            ABCHECK(nullptr != CharacterWidget);
            // UI 위젯을 캐릭터의 StatComponenet와 연동
            CharacterWidget->BindCharacterStat(CharacterStat);
            break;
        }
        // DEAD state : 캐릭터가 HP를 소진해 사망할 때 발생하는 state
        case ECharacterState::DEAD:
        {
            SetActorEnableCollision(false);
            GetMesh()->SetHiddenInGame(false);
            HPBarWidget->SetHiddenInGame(true);
            ABAnim->SetDeadAnim();
            SetCanBeDamaged(false);
            break;
        }
        }
    }

    ECharacterState AABCharactter::GetCharacterState() const
    {
        return CurrentState;
    }
    ...
    void AABCharactter::OnAssetLoadCompleted()
    {
        // 스켈레톤 메시에 불러온 애셋 선언
        USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
        // 핸들러 초기화
        AssetStreamingHandle.Reset();
        ABCHECK(nullptr != AssetLoaded);
        // 메시 세팅
        GetMesh()->SetSkeletalMesh(AssetLoaded);
        // 로딩이 끝나면 state를 READY로 변경해 게임 시작
        SetCharacterState(ECharacterState::READY);
    }
    ...

    Behavior Tree 로직을 수동으로 조작할 수 있게 변경
    ABAIController.h
    ...
    // state에 맞게 behavior tree 로직을 수동으로 구동/중지
	void RunAI();
	void StopAI();
    ...
    ABAIController.cpp
    ...
    void AABAIController::OnPossess(APawn* InPawn)
    {
        Super::OnPossess(InPawn);
    }

    void AABAIController::RunAI()
    {
        UBlackboardComponent* BlackboardComponent = Blackboard;
        // AI가 생성한 Blackboard 사용하도록 한다.
        if (UseBlackboard(BBAsset, BlackboardComponent))
        {
            // Behavior Tree 실행 전 Blackboard 데이터 세팅
            BlackboardComponent->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());

            // 생성한 Behavior Tree 애셋이 블랙보드 앳세과 함께 동작
            if (!RunBehaviorTree(BTAsset))
            {
                ABLOG(Error, TEXT("AIController couldn't run behavior tree!"));
            }
        }
    }

    void AABAIController::StopAI()
    {
        // Behavior Tree Component 가져오기
        auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
        if (nullptr != BehaviorTreeComponent)
        {
            // 로직 멈추기
            BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
        }
    }
    ...

    캐릭터의 PossessedBy 함수 대신 READY state에서 빙의를 구현하고 입력과
    Behavior Tree System을 활성화 해준다.
    Dead State에선 입력과 Behavior Tree System을 비활성화 해주고
    타이머를 발동시켜 사망한 이후에 처리할 로직도 구현한다.
    ABCharacter.h
    ...
    // 사망한 이후 타이머
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
	float DeadTimer;

	FTimerHandle DeadTimerHandle = {};
    ...
    ABCharacter.cpp
    ...생산자
    // 타이머 5초 설정
    DeadTimer = 5.f;
    ...
    void AABCharactter::SetCharacterState(ECharacterState NewState)
    {
        ...
        switch (CurrentState)
        {
        // LOADING state : 선택한 캐릭터 애셋을 로딩하는 state
        case ECharacterState::LOADING:
        {
            if (bIsPlayer)
            {	
                // LOADING state이므로 입력 비활성화
                DisableInput(ABPlayerController);
            }
            ...
            break;
        }
        // READY state : 캐릭터 애셋 로딩이 완료된 state
        case ECharacterState::READY:
        {
            ...
            if (bIsPlayer)
            {
                // 게임 시작 시 입력 활성화 및 카메라 세팅
                SetControlMode(EControlMode::QUARTERVIEW);
                GetCharacterMovement()->MaxWalkSpeed = 600.f;
                EnableInput(ABPlayerController);
            }
            else
            {
                SetControlMode(EControlMode::NPC);
                GetCharacterMovement()->MaxWalkSpeed = 400.f;
                // NPC인 경우 BehaviorTree 활성화
                ABAIController->RunAI();
            }
            break;
        }
        // DEAD state : 캐릭터가 HP를 소진해 사망할 때 발생하는 state
        case ECharacterState::DEAD:
        {
            ...
            if (bIsPlayer)
            {
                DisableInput(ABPlayerController);
            }
            else
            {
                ABAIController->StopAI();
            }

            GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
                if (bIsPlayer)
                {
                    // 플레이어가 사망하면 게임 재시작
                    ABPlayerController->RestartLevel();
                }
                else
                {
                    // NPC가 사망하면 레벨에서 퇴장
                    Destroy();
                }
                }), DeadTimer, false
            );
            break;
        }
        }
    }

*/