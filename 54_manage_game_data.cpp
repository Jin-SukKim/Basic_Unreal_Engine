/*
    게임 데이터의 관리

    플레이어 데이터를 선언하고 이를 HUD UI의 왼쪽 상단 부분과 연동하는 기능을 구현했다.
    이번엔 게임의 스코어 부분을 구현해본다.

    게임 시작시 플레이어는 0의 점수를 가진다.
    싱글플레이이기에 NPC를 처치하면 점수를 1의 점수를 획득한다.

    멀티플레이 콘텐츠는 두 개의 서로 다른 의미를 가질 수 있다.
    각 플레이어의 점수를 서로 별도로 관리해줘야해 점수를 어떻게 설정할 것인가에 대한 기획이 필요하다.

    즉, 멀티 플레이까지 고려하면 플레이어에 설정된 데이터 외에도 게임의 데이터를
    관리하는 기능을 추가로 고려해야 한다.
    UE은 이를 관리하도록 GameState라는 클래스를 제공한다.

    GameStateBase 클래스를 Parent로 하는 ABGameState 클래스를 생성한다.

    새로운 GameSTate를 생성하고 나면 전체 게임 스코어를 저장하는 속성을 추가한다.

    ABGameState.h
    ...
    public:
        AABGameState();

        int32 GetTotalGameScore() const;
        void AddGameScore();

    private:
        // 전체 게임 스코어
        UPROPERTY(Transient)
        int32 TotalGameScore;
    ...
    ABGameState.cpp
    ...
    AABGameState::AABGameState()
    {
        TotalGameScore = 0;
    }

    int32 AABGameState::GetTotalGameScore() const
    {
        return TotalGameScore;
    }

    void AABGameState::AddGameScore()
    {
        TotalGameScore++;
    }
    ...
    ABGameMode.cpp
    ... 생산자
    GameStateClass = AABGameState::StaticClass();
    ...

    이제 섹션 액터의 로직으로 이동해 섹션에서 생성한 NPC가 플레이어에 의해 제거되면,
    이를 감지해 섹션 액터의 state를 COMPLETE로 변경하는 기능을 추가한다.
    NPC가 제거될 때 마지막으로 대미지를 입힌 Controller의 기록은 LastHitBy 속성에 저장된다.
    이를 사용해 액터가 제거될 때 마지막에 피격을 가한 플레이어의 정보를 바로 얻어올 수 있다.

    이전에 구현한 경험치의 경우 대미지를 입르 때마다 instigator를 검사하는 방식보다 소멸될 때 LastHitBy를
    사용해 처리하는 것이 효율적이지만, 두가지 경우 모두 만들어봤다.
    ABSection.h
    ...
    // 액터가 제거될 때 호출되는 함수
	UFUNCTION()
	void OnKeyNPCDestroyed(AActor* DestroyedActor);
    ...
    ABSection.cpp
    ...
    void AABSection::OnNPCSpawn()
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnNPCTimerHandle);
        // 스폰된 NPC
        auto KeyNPC = GetWorld()->SpawnActor<AABCharactter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator);
        if (nullptr != KeyNPC)
        {
            KeyNPC->OnDestroyed.AddDynamic(this, &AABSection::OnKeyNPCDestroyed);
        }
    }

    void AABSection::OnKeyNPCDestroyed(AActor* DestroyedActor)
    {
        // 파괴된 NPC 가져온다.
        auto ABCharacter = Cast<AABCharactter>(DestroyedActor);
        ABCHECK(nullptr != ABCharacter);

        // LastHitBy 는 마지막으로 대미지를 입힌 Controller의 정보가 저장되 있다.
        auto ABPlayerController = Cast<AABPlayerController>(ABCharacter->LastHitBy);
        ABCHECK(nullptr != ABPlayerController);

        // NPC가 파괴되었기에 섹션을 Complete로 변환해준다.
        SetState(ESectionState::COMPLETE);
    }
    ...

    NPC를 제거해 섹션을 클리어하면 게임 모드에서 스코어를 올리라는 명령을 내린다.
    마지막 피격을 가한 PlayerController 정보를 함께 넘겨 PlayerState의 점수를 높이고,
    전체 스코어에 해당하는 GameState의 Score도 같이 올린다.

    게임에 참여중인 플레이어 컨트롤러의 목록은 월드에서 제공하는 GetPlayerControllerIterator를 사용해 얻을 수 있따.
    ABPlayerState.h
    ...
    void AddGameScore();
    ...
    ABPlayerState.cpp
    ...
    void AABPlayerState::AddGameScore()
    {
        GameScore++;
        OnPlayerStateChanged.Broadcast();
    }
    ...
    ABPlayerController.h
    ...
    void AddGameScore() const;
    ...
    ABPlayerController.cpp
    ...
    void AABPlayerController::AddGameScore() const
    {
        ABPlayerState->AddGameScore();
    }
    ...
    ABGameMode.h
    ...
    	virtual void PostInitializeComponents() override;
        void AddScore(class AABPlayerController* ScoredPlayer);

    private:
        UPROPERTY()
        class AABGameState* ABGameState;
    ...
    ABGameMode.cpp
    ...
    void AABGameMode::PostInitializeComponents()
    {
        Super::PostInitializeComponents();
        ABGameState = Cast<AABGameState>(GameState);
    }

    void AABGameMode::AddScore(AABPlayerController* ScoredPlayer)
    {	
        // 게임의 PlayerController 목록을 GetPlayerControllerIterator() 함수로 가져올 수 있다.
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
            // 점수를 얻은 플레이어가 누군지 확인
            if ((nullptr != ABPlayerController) && (ScoredPlayer == ABPlayerController))
            {
                // 플레이어 게임 점수 추가
                ABPlayerController->AddGameScore();
                break;
            }
        }

        // 전체 게임 점수 추가
        ABGameState->AddGameScore();
    }
    ...
    ABSection.cpp
    ...
    void AABSection::OnKeyNPCDestroyed(AActor* DestroyedActor)
    {
        // 파괴된 NPC 가져온다.
        auto ABCharacter = Cast<AABCharactter>(DestroyedActor);
        ABCHECK(nullptr != ABCharacter);

        // LastHitBy 는 마지막으로 대미지를 입힌 Controller의 정보가 저장되 있다.
        auto ABPlayerController = Cast<AABPlayerController>(ABCharacter->LastHitBy);
        ABCHECK(nullptr != ABPlayerController);

        // 현재 게임 모드 가져오기
        auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
        ABCHECK(nullptr != ABGameMode);
        ABGameMode->AddScore(ABPlayerController);

        // NPC가 파괴되었기에 섹션을 Complete로 변환해준다.
        SetState(ESectionState::COMPLETE);
    }
    ...

    이러면 점수가 올라갈텐데 기본적인 게임플레이의 구조를 완성한 것이다.ㄴ
*/