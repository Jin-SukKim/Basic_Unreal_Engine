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
*/