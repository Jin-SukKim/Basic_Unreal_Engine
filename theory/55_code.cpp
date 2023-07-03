/*
    맨 처음 데이터 저장 시 기본 세이브 데이터를 생성하는 로직
    ABSaveGame.h
    ...
    public:
        UABSaveGame();

        UPROPERTY()
        int32 Level;
        
        UPROPERTY()
        int32 Exp;

        UPROPERTY()
        FString PlayerName;

        UPROPERTY()
        int32 HighScore;
    ...
    ABSaveGame.cpp
    ...
    UABSaveGame::UABSaveGame()
    {
        Level = 1;
        Exp = 0;
        PlayerName = TEXT("Guest");
        HighScore = 0;
    }
    ...
    ABPlayerState.h
    ...
    int32 GetGameHighScore() const;
	// 저장된 게임 데이터 파일에 접근하기 위한 고유의 슬롯 이름
	FString SaveSlotName;
    ...
    UPROPERTY(Transient)
	int32 GameHighScore;
    ...
    ABPlayerState.cpp
    ...
    AABPlayerState::AABPlayerState()
    {
        ...
        GameHighScore = 0;
        SaveSlotName = TEXT("Player1");
    }
    ...
    void AABPlayerState::InitPlayerData()
    {
        // 맨 처음 세이브 데이터 파일이 없으면 기본 세이브 파일 생성
        auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
        if (nullptr == ABSaveGame)
        {
            ABSaveGame = GetMutableDefault<UABSaveGame>();
        }
        SetPlayerName(ABSaveGame->PlayerName);
        SetCharacterLevel(ABSaveGame->Level);
        GameScore = 0;
        GameHighScore = ABSaveGame->HighScore;
        Exp = 0;
    }
    int32 AABPlayerState::GetGameHighScore() const
    {
        return GameHighScore;
    }
    void AABPlayerState::AddGameScore()
    {
        GameScore++;
        // 최고점 갱신
        if (GameScore >= GameHighScore)
        {
            GameHighScore = GameScore;
        }
        OnPlayerStateChanged.Broadcast();
    }
    ...

    최초에 플레이어 데이터를 생성한 후 바로 저장하고, 경험치를 획득할 때마다 저장

    ABPlayerState.h
    ...
    // 플레이어 데이터 저장
	void SavePlayerData();
    ...
    ABPlayerState.cpp
    ...
    void AABPlayerState::InitPlayerData()
    {
        ...
        // 최초에 플레이어 데이터 생성하자마자 바로 저장
	    SavePlayerData();
    }  
    ...
    void AABPlayerState::SavePlayerData()
    {
        // 새로운 세이브 데이터 언리얼 오브젝트
        UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
        NewPlayerData->PlayerName = GetPlayerName();
        NewPlayerData->Level = CharacterLevel;
        NewPlayerData->Exp = Exp;
        NewPlayerData->HighScore = GameHighScore;

        // Slot Name을 기준으로 slot에 데이터 저장하기
        if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0))
        {
            ABLOG(Error, TEXT("SaveGame Error!"));
        }
    }
    
    bool AABPlayerState::AddExp(int32 IncomeExp)
    {
        ...
        // 경험치 획득할 때마다 데이터 저장
        SavePlayerData();
        return DidLevelUp;
    }

    void AABPlayerState::AddGameScore()
    {
        ...
        // 점수 획득할 때마다 데이터 저장
        SavePlayerData();
    }

    PlayerState의 Highsocre 값을 HUD UI에 연동시킨다.

    ABHUDWidget.cpp
    ...
    void UABHUDWidget::UpdatePlayerState()
    {
        ABCHECK(CurrentPlayerState.IsValid());

        ExpBar->SetPercent(CurrentPlayerState->GetExpRatio());
        PlayerName->SetText(FText::FromString(CurrentPlayerState->GetPlayerName()));
        PlayerLevel->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetCharacterLevel())));
        CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameScore())));
        ABLOG(Warning, TEXT("Score : %f"), CurrentPlayerState->GetGameScore());
        HighScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameHighScore())));
    }
*/