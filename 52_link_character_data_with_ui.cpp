/*
    플레이어 데이터와 UI 연동

    캐릭터의 전투에서 사용할 스탯 정보를 액터 컴포넌트로 분리해 구현했었따.
    이와 같잉 게임 점수와 같은 플레이어의 게임 데이터를 별도의 액터에서 보관하도록 한다.

    UE은 플레이어의 정볼르 관리하기 위한 용도로 PlayerState라는 클래스를 제공한다.
    PlayerSTate를 상속받은 ABPlayerState라는 클래스를 생성한다.

    PlayerState 클래스에는 FString형의 PlayerName 속성과 float 형의 Score 속성이 이미 설계돼 있다.
    이 속성은 게임에서 보여질 플레이어의 별명(Nickname)과 점수를 관리하기 위한 용도로 사용된다.

    게임의 진행 상황을 기록하도록 별도로 int32형으로 GameScore라는 속성을 추가하고,
    플레이어의 레벨 정보를 표시하기 위한 int32형의 CharacterLevel이라는 속성을 추가한다.

    생성한 ABPlayerState 클래스를 게임 모드의 PlayerSTateClass 속성에 지정하면 
    엔진에서는 플레이어 컨트롤러가 초기화될 때 함께 해당 클래스의 인스턴스를 생성하고
    그 포인터 값을 플레이어 컨트롤러의 PlayerState 속성에 저장된다.

    플레이어 컨트롤러의 구성을 완료하는 시점은 게임 모드의 postLogin 함수이므로 이때
    함께 ABPlayerState의 초기화도 완료해주는 것이 좋다.

    ABPlayerState의 설계와 게임 모드의 PostLogin에서 새로운 플레이어 state의 초기화를 진행한 코드
    ABPlayerState.h
    ...
    public:
        AABPlayerState();

        int32 GetGameScore() const;
        int32 GetCharacterLevel() const;

        void InitPlayerData();

    protected:
        UPROPERTY(Transient)
        int32 GameScore;

        UPROPERTY(Transient)
        int32 CharacterLevel;
    ...
    ABPlayerState.cpp
    AABPlayerState::AABPlayerState()
    {
        CharacterLevel = 1;
        GameScore = 0;
    }

    int32 AABPlayerState::GetGameScore() const
    {
        return GameScore;
    }

    int32 AABPlayerState::GetCharacterLevel() const
    {
        return CharacterLevel;
    }

    void AABPlayerState::InitPlayerData()
    {
        SetPlayerName(TEXT("Destiny"));
        CharacterLevel = 5;
        GameScore = 0;
    }
    ABGameMode.cpp
    AABGameMode::AABGameMode()
    {
        // 제작한 캐릭터를 기본 폰으로 설정
        DefaultPawnClass = AABCharactter::StaticClass();
        PlayerControllerClass = AABPlayerController::StaticClass();
        PlayerStateClass = AABPlayerState::StaticClass();

    }

    void AABGameMode::PostLogin(APlayerController* newPlayer)
    {
        ABLOG(Warning, TEXT("PostLogin Begin"));
        Super::PostLogin(newPlayer);
        ABLOG(Warning, TEXT("PostLogin End"));

        auto ABPlayerState = Cast<AABPlayerState>(newPlayer->PlayerState);
        ABCHECK(nullptr != ABPlayerState);
        ABPlayerState->InitPlayerData();
    }

    플레이어의 레벨 정보는 실제로 캐릭터에 반영해야 하므로 플레이어 컨트롤러가
    캐릭터에 빙의할 때 캐릭터의 PlayerState 속성에 플레이어 state의 포인터를 저장하므로
    캐릭터에서도 해당 Playerstate 정보를 바로 가져올 수 있다.
    ABCharacter.cpp
    
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

                auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
                ABCHECK(nullptr != ABPlayerState);
                CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());
            }
    ...

    이러면 캐릭터는 5레벨로 설정된다.
*/