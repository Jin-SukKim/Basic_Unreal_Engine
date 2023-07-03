/*
    HUD UI를 플레이어에게 띄우기

    ABPlayerController.h
    ...
	// HUD 위젯 애셋의 클래스 정보
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABHUDWidget> HUDWidgetClass;

    private:
        UPROPERTY()
        class UABHUDWidget* HUDWidget;
    ...
    ABPlayerController.cpp
    #include "ABHUDWidget.h"
    AABPlayerController::AABPlayerController()
    {
        static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
        if (UI_HUD_C.Succeeded())
        {
            HUDWidgetClass = UI_HUD_C.Class;
        }
    }
    ...
    UABHUDWidget* AABPlayerController::GetHUDWidget() const
    {
        return HUDWidget;
    }

    void AABPlayerController::BeginPlay()
    {
        Super::BeginPlay();

        FInputModeGameOnly InputMode;
        SetInputMode(InputMode);

        HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
        HUDWidget->AddToViewport();
    }
    ...

    PlayerState와 Character Stat Component 정보를 Delegate를 사용해 모두 해당 HUD에 연동해준다.
    Delegate는 데이터가 변경될 때 HUD 관련 위젯을 업데이트 해준다.

    ABPlayerState.h
    ... 
    DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangedDelegate);
    ...
    // Player Data와 연동을 위한 Delegate
	FOnPlayerStateChangedDelegate OnPlayerStateChanged;
    ...
    ABHUDWidget.h
    ...
    public:
        // CharacterStat과 연동
        void BindCharacterStat(class UABCharacterStatComponent* CharacterStat);
        // PlayerState와 연동
        void BindPlayerState(class AABPlayerState* PlayerState);

    protected:
        virtual void NativeConstruct() override;
        void UpdateCharacterStat();
        void UpdatePlayerState();

    private:
        TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat;
        TWeakObjectPtr<class AABPlayerState> CurrentPlayerState;

        UPROPERTY()
        class UProgressBar* HPBar;

        UPROPERTY()
        class UProgressBar* ExpBar;

        UPROPERTY()
        class UTextBlock* PlayerName;

        UPROPERTY()
        class UTextBlock* PlayerLevel;

        UPROPERTY()
        class UTextBlock* CurrentScore;

        UPROPERTY()
        class UTextBlock* HighScore;
    ...
    ABHUDWidget.cpp
    #include "Components/TextBlock.h"
    #include "Components/ProgressBar.h"
    #include "ABCharacterStatComponent.h"
    #include "ABPlayerState.h"

    void UABHUDWidget::BindCharacterStat(UABCharacterStatComponent* CharacterStat)
    {
        ABCHECK(nullptr != CharacterStat);
        CurrentCharacterStat = CharacterStat;
        // CharacterStat의 HPChanged Delegate와 연동
        CharacterStat->OnHPChanged.AddUObject(this, &UABHUDWidget::UpdateCharacterStat);
    }

    void UABHUDWidget::BindPlayerState(AABPlayerState* PlayerState)
    {
        ABCHECK(nullptr != PlayerState);
        CurrentPlayerState = PlayerState;
        PlayerState->OnPlayerStateChanged.AddUObject(this, &UABHUDWidget::UpdatePlayerState);
    }

    void UABHUDWidget::NativeConstruct()
    {
        Super::NativeConstruct();
        HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbHP")));
        ABCHECK(nullptr != HPBar);

        ExpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbExp")));
        ABCHECK(nullptr != ExpBar);

        PlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtPlayerName")));
        ABCHECK(nullptr != PlayerName);

        PlayerLevel = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtLevel")));
        ABCHECK(nullptr != PlayerLevel);

        CurrentScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtCurrentScore")));
        ABCHECK(nullptr != CurrentScore);

        HighScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtHighScore")));
        ABCHECK(nullptr != HighScore);
    }

    void UABHUDWidget::UpdateCharacterStat()
    {
        ABCHECK(CurrentCharacterStat.IsValid());
        // ProgressBar 업데이트
        HPBar->SetPercent(CurrentCharacterStat->GetHPRatio());
    }

    void UABHUDWidget::UpdatePlayerState()
    {
        ABCHECK(CurrentPlayerState.IsValid());

        PlayerName->SetText(FText::FromString(CurrentPlayerState->GetPlayerName()));
        PlayerLevel->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetCharacterLevel())));
        CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetScore())));
    }

    PlayerState와 CharacterStatComponent를 HUD와 연결해준다.

    ABPlayerController.cpp
    ...
    void AABPlayerController::BeginPlay()
    {
        ...
        // HUD위젯과 PlayerState 연결
        auto ABPlayerState = Cast<AABPlayerState>(PlayerState);
        ABCHECK(nullptr != ABPlayerState);
        HUDWidget->BindPlayerState(ABPlayerState);
        ABPlayerState->OnPlayerStateChanged.Broadcast();
        ...
    }
    ...
    ABCharacter.cpp
    ...
    void AABCharacter::SetCharacterState(ECharacterState NewState)
    {
        ...
        case ECharacterState::LOADING:
        {
            if (bIsPlayer)
            {
                ...
                // HUD위젯과 캐릭터 스탯 컴포넌트를 연결
			    ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);
                ...
            }
        }
    }


    CharacterStat에 NPC를 위한 경험치 값 설정,
    PlayerState에 플레이어 경험치 데이터 보관하도록 설계를 확장,
    캐릭터 사망 시 NPC가 플레이어에게 죽는지 검사하고 해당 PlayerController를 통해 PlayerState를 업데이트하는 로직을 추가
    ABPlayerState.h
    ...
        // 경험치
        float GetExpRatio() const;
        bool AddExp(int32 IncomeExp);
        ...
        UPROPERTY(Transient)
        int Exp;

    private:
        void SetCharacterLevel(int32 NewCharacterLevel);
        struct FABCharacterData* CurrentStatData;
    ...
    ABPlayerState.cpp
    ...
    AABPlayerState::AABPlayerState()
    {
        CharacterLevel = 1;
        GameScore = 0;
        Exp = 0;
    }
    void AABPlayerState::InitPlayerData()
    {
        SetPlayerName(TEXT("Destiny"));
        SetCharacterLevel(5);
        GameScore = 0;
        Exp = 0;
    }

    float AABPlayerState::GetExpRatio() const
    {
        // 얻을 수 있는 Exp가 0에 가까운 경우
        if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
            return 0.f;

        // UI의 ProgressBar에 표시되므로 Ratio 계산
        float Result = (float)Exp / (float)CurrentStatData->NextExp;
        ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
        return Result;
    }

    bool AABPlayerState::AddExp(int32 IncomeExp)
    {
        if (CurrentStatData->NextExp == -1)
            return false;

        bool DidLevelUp = false;
        // 현재 Exp에 얻은 Exp 더해주기
        Exp = Exp + IncomeExp;
        // 충분한 경험치를 얻었다면 레벨업
        if (Exp >= CurrentStatData->NextExp)
        {
            Exp -= CurrentStatData->NextExp;
            SetCharacterLevel(CharacterLevel + 1);
            DidLevelUp = true;
        }

        OnPlayerStateChanged.Broadcast();
        return DidLevelUp;
    }

    void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
    {
        auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
        ABCHECK(nullptr != ABGameInstance);

        CurrentStatData = ABGameInstance->GetABCHaracterData(NewCharacterLevel);
        ABCHECK(nullptr != CurrentStatData);

        CharacterLevel = NewCharacterLevel;
    }

    ABHUDWidget.cpp
    ...
    void UABHUDWidget::UpdatePlayerState()
    {
        ABCHECK(CurrentPlayerState.IsValid());

        ExpBar->SetPercent(CurrentPlayerState->GetExpRatio());
        PlayerName->SetText(FText::FromString(CurrentPlayerState->GetPlayerName()));
        PlayerLevel->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetCharacterLevel())));
        CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetScore())));
    }
    ...
    ABCharacterStatComponent.h
    ...
    int32 GetDropExp() const;
    ...
    ABCharacterStatComponent.cpp
    ...
    int32 UABCharacterStatComponent::GetDropExp() const
    {
        return CurrentStatData->DropExp;
    }
    ...
    ABCharacter.h
    ...
    int32 GetExp() const;
    ...
    ABCharacter.cpp
    ...
    int32 AABCharactter::GetExp() const
    {
        return CharacterStat->GetDropExp();
    }

    float AABCharactter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
    {
        float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
        ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

        // StatComponent 클래스를 사용해 대미지 계산
        CharacterStat->SetDamage(FinalDamage);

        if (CurrentState == ECharacterState::DEAD)
        {
            // 대미지 framework에서 PlayerController의 정보는 가해자(Instigator) 인자로 전달
            // 즉, 현재 캐릭터 액터가 NPC인 경우이다.
            if (EventInstigator->IsPlayerController())
            {
                auto PlayerController = Cast<AABPlayerController>(EventInstigator);
                ABCHECK(nullptr != PlayerController, 0.f);
                PlayerController->NPCKill(this);
            }
        }
        return FinalDamage;
    }
    ...
    ABPlayerController.h
    ...
    // 플레이어가 NPC를 죽였는지 확인
	void NPCKill(class AABCharacter* KilledNPC) const;
    ...
    UPROPERTY()
	class AABPlayerState* ABPlayerState;
    ...
    ABPlayerController.cpp
    ...
    void AABPlayerController::BeginPlay()
    {
        Super::BeginPlay();

        FInputModeGameOnly InputMode;
        SetInputMode(InputMode);

        HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
        HUDWidget->AddToViewport();

        // HUD위젯과 PlayerState 연결
        ABPlayerState = Cast<AABPlayerState>(PlayerState);
        ABCHECK(nullptr != ABPlayerState);
        HUDWidget->BindPlayerState(ABPlayerState);
        ABPlayerState->OnPlayerStateChanged.Broadcast();
    }
    
    void AABPlayerController::NPCKill(AABCharactter* KilledNPC) const
    {
        ABPlayerState->AddExp(KilledNPC->GetExp());
    }

*/