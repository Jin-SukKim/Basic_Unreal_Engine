/*

    액션 매핑으로 M키로 게임 중단 UI를 열도록 PlayerController에서 바인딩해준다.

    ABPlayerController.h
    ...
    // 플레이어 컨트롤러에 입력 매핑
	virtual void SetupInputComponent() override;
    ...
    void OnGamePause();
    ...
    ABPlayerController.cpp
    ...
    AABPlayerController::AABPlayerController()
    {
        static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
        if (UI_HUD_C.Succeeded())
        {
            HUDWidgetClass = UI_HUD_C.Class;
        }

        static ConstructorHelpers::FObjectFinder<UInputMappingContext> DEFAULT_CONTEXT(TEXT("/Game/Book/ABInputMappingContext.ABInputMappingContext"));
        if (DEFAULT_CONTEXT.Succeeded())
        {
            InputMapping = DEFAULT_CONTEXT.Object;
        }

        static ConstructorHelpers::FObjectFinder<UInputAction> PAUSE_ACTION(TEXT("/Game/Book/ABPause.ABPause"));
        if (PAUSE_ACTION.Succeeded())
        {
            GamePause = PAUSE_ACTION.Object;
        }
    }
    void AABPlayerController::PostInitializeComponents()
    {
        Super::PostInitializeComponents();
        ABLOG_S(Warning);

        // 컨트롤러 가져오기
        auto ABPlayerController = Cast<AABPlayerController>(this);
        if (ABPlayerController)
        {
            ABCHECK(nullptr != ABPlayerController);
            // enhancde input의 서브시스템
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ABPlayerController->GetLocalPlayer()))
            {
                Subsystem->AddMappingContext(InputMapping, 0);
            }
        }
    }
    void AABPlayerController::SetupInputComponent()
    {
        Super::SetupInputComponent();
        
        // UEnhancedInputComponent를 사용하는지 확인해야 합니다. 그러지 않으면 프로젝트가 제대로 설정되지 않습니다.
        if (UEnhancedInputComponent* PlayerEnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
        {
            // Move()를 MoveAction인 InputAction에 bind해준다.
            PlayerEnhancedInputComponent->BindAction(GamePause, ETriggerEvent::Started, this, &AABPlayerController::OnGamePause);
        }
    }
    ...

    ABGameplayWidget이란 UserWidget을 부모로하는 클래스를 생성하고 UI_Menu 애셋의 부모 클래스로 설정해준다.
    UI 위젯을 초기화하는 NativeContruct 함수에서 버튼을 바인딩해준다.

    ABGameplayWidget.h
    ...
    protected:
        // UI 위젯을 초기화하는 시점에서 발생하는 함수
        virtual void NativeConstruct() override;

        UFUNCTION()
        void OnResumeClicked();

        UFUNCTION()
        void OnReturnToTitleClicked();

        UFUNCTION()
        void OnRetryGameClicked();

        UPROPERTY()
        class UButton* ResumeButton;

        UPROPERTY()
        class UButton* ReturnToTitleButton;

        UPROPERTY()
        class UButton* RetryGameButton;
    ...
    ABGameplayWidget.cpp
    ...
    #include "Components/Button.h"
    // 이름으로 버튼을 찾고, 해당 이름의 버튼이 존재하면 바인딩 구현
    void UABGameplayWidget::NativeConstruct()
    {
        Super::NativeConstruct();

        ResumeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnResume")));
        if (nullptr != ResumeButton)
        {
            ResumeButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnResumeClicked);
        }

        ReturnToTitleButton = Cast<UButton>(GetWidgetFromName(TEXT("btnReturnToTitle")));
        if (nullptr != ReturnToTitleButton)
        {
            ReturnToTitleButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnReturnToTitleClicked);
        }

        RetryGameButton = Cast<UButton>(GetWidgetFromName(TEXT("btnRetryGame")));
        if (nullptr != RetryGameButton)
        {
            RetryGameButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnRetryGameClicked);
        }
    }

    void UABGameplayWidget::OnResumeClicked()
    {
    }

    void UABGameplayWidget::OnReturnToTitleClicked()
    {
    }

    void UABGameplayWidget::OnRetryGameClicked()
    {
    }
    ...

    M 버튼을 눌렀을 때 메뉴 UI가 나타나도록 한다.
    메뉴 UI 나오면 게임플레이의 중지, 마우스 커서 보여주기, 입력이 게임에 전달되지 않고 UI에만 전달되도록 제어한다.

    ABPlayerController.h
    ...
    public :
        ...
        void ChangeInputMode(bool bGameMode = true);
        ...
    protected:
        ...
        UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	    TSubclassOf<class UABGameplayWidget> MenuWidgetClass;
        ...
    private:
        ...
        UPROPERTY()
        class UABGameplayWidget* MenuWidget;

        FInputModeGameOnly GameInputMode;
        FInputModeUIOnly UIInputMode;
    ...
    ABPlayerController.cpp
    ...
    AABPlayerController::AABPlayerController()
    {
        ...
        static ConstructorHelpers::FClassFinder<UABGameplayWidget> UI_MENU_C(TEXT("/Game/Book/UI/UI_Menu.UI_Menu_C"));
        if (UI_MENU_C.Succeeded())
        {
            MenuWidgetClass = UI_MENU_C.Class;
        }
    }

    void AABPlayerController::PostInitializeComponents()
    {
        Super::PostInitializeComponents();
        ABLOG_S(Warning);
    }
    void AABPlayerController::ChangeInputMode(bool bGameMode)
    {
        // UI에만 마우스 입력을 전달할 수 있도록 설계
        if (bGameMode)
        {
            SetInputMode(GameInputMode);
            bShowMouseCursor = false;
        }
        else
        {
            SetInputMode(UIInputMode);
            bShowMouseCursor = true;

        }
    }
    ...
    void AABPlayerController::BeginPlay()
    {
        Super::BeginPlay();

        // 컨트롤러 가져오기
        auto ABPlayerController = Cast<AABPlayerController>(this);
        if (ABPlayerController)
        {
            ABCHECK(nullptr != ABPlayerController);
            // enhancde input의 서브시스템
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ABPlayerController->GetLocalPlayer()))
            {
                Subsystem->AddMappingContext(InputMapping, 0);
            }
        }

        ChangeInputMode(true);

        HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
        ABCHECK(nullptr != HUDWidget);
        HUDWidget->AddToViewport(1);
        ...
    }
    ...
    void AABPlayerController::OnGamePause()
    {
        MenuWidget = CreateWidget<UABGameplayWidget>(this, MenuWidgetClass);
        ABCHECK(nullptr != MenuWidget);
        MenuWidget->AddToViewport(3);
        
        SetPause(true);
        ChangeInputMode(false);
    }
    ...

    이제 중지 메뉴 UI의 버튼 기능을 구현해준다.
    ABGameplayWidget.cpp
    ...
    // 이름으로 버튼을 찾고, 해당 이름의 버튼이 존재하면 바인딩 구현
    void UABGameplayWidget::NativeConstruct()
    {
        Super::NativeConstruct();

        ResumeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnResume")));
        if (nullptr != ResumeButton)
        {
            ResumeButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnResumeClicked);
        }

        ReturnToTitleButton = Cast<UButton>(GetWidgetFromName(TEXT("btnReturnToTitle")));
        if (nullptr != ReturnToTitleButton)
        {
            ReturnToTitleButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnReturnToTitleClicked);
        }

        RetryGameButton = Cast<UButton>(GetWidgetFromName(TEXT("btnRetryGame")));
        if (nullptr != RetryGameButton)
        {
            RetryGameButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnRetryGameClicked);
        }
    }

    void UABGameplayWidget::OnResumeClicked()
    {
        // UI는 GetOwningPlayer 함수를 사용해 현재 자신을 생성하고 관리하는 PlayerController의 정보를 가져올 수 있다.
        auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer());
        ABCHECK(nullptr != ABPlayerController);

        // 현재 뷰포트에 띄워진 자신을 제거할 수 있는 함수
        RemoveFromParent();

        // 입력 모드는 UI에서 게임으로 바꿔준다.
        ABPlayerController->ChangeInputMode(true);
        ABPlayerController->SetPause(false);
    }

    void UABGameplayWidget::OnReturnToTitleClicked()
    {
        // 타이틀로 돌아가게 Title Level을 가져온다.
        UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
    }

    void UABGameplayWidget::OnRetryGameClicked()
    {
    }
    ...

    이게 결과 UI를 표시하는 기능을 구현하고 UI의 기능들을 구현해준다.
    ABGameplayResultWidget.h
    ...
    protected:
	    virtual void NativeConstruct() override;
    ...
    ABGameplayResultWidget.cpp
    ...
    void UABGameplayResultWidget::NativeConstruct()
    {
        Super::NativeConstruct();

        auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult")));
        ABCHECK(nullptr != Result);

        auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore")));
        ABCHECK(nullptr != TotalScore);
    }
    ...
    ABPlayerController.h
    ...
    public:
        ...
        // 게임이 종료되면 UI를 띄워즌다.
        void ShowResultUI();
    protected:
        ...
        UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	    TSubclassOf<class UABGameplayResultWidget> ResultWidgetClass;
    private:
        UPROPERTY()
	    class UABGameplayResultWidget* ResultWidget;
        ...
    ABPlayerController.cpp
    ... 생산자.
    static ConstructorHelpers::FClassFinder<UABGameplayWidget> UI_RESULT_C(TEXT("/Game/Book/UI/UI_Result.UI_Result_C"));
	if (UI_RESULT_C.Succeeded())
		ResultWidgetClass = UI_RESULT_C.Class;
    ...
    void AABPlayerController::BeginPlay()
    {   
        ...
        // 미리 Result UI를 생성해둔다.
        ResultWidget = CreateWidget<UABGameplayResultWidget>(this, ResultWidgetClass);
	    ABCHECK(nullptr != ResultWidget);
        ...
    }
    ...
    void AABPlayerController::ShowResultUI()
    {
        ResultWidget->AddToViewport();
        ChangeInputMode(false);
    }
    ...

    UI 관련 기능을 구현하면 게임플레이가 종료되는 시점을 지정해준다.
    ABGameState.h
    ...
    // 게임이 종료되는 두가지 시점(1. 미션 클리어, 2. 플레이어 죽음)
	void SetGameCleared();
	bool IsGameCleared() const;
    ...
    UPROPERTY(Transient)
	bool bGameCleared;
    ...
    ABGameState.cpp
    ...
    AABGameState::AABGameState()
    {
        TotalGameScore = 0;
        bGameCleared = false;
    }
    ...
    void AABGameState::SetGameCleared()
    {
        bGameCleared = true;
    }
    bool AABGameState::IsGameCleared() const
    {
        return bGameCleared;
    }
    ...
    ABGameMode.h
    ...
    private:
        ...
        UPROPERTY()
        int32 ScoreToClear;
        ...
    ABGameMode.cpp
    ...생산자
    ScoreToClear = 2;
    ...
    void AABGameMode::AddScore(AABPlayerController* ScoredPlayer)
    {	
        ...
        // 미션 클리어해 게임이 종료되면 동작하는 모든 폰을 멈추고 bGameClear 속성을 true로 설정해준다.
        if (GetScore() >= ScoreToClear)
        {
            ABGameState->SetGameCleared();

            for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
            {
                (*It)->TurnOff();
            }

            for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
            {
                const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
                if (nullptr != ABPlayerController)
                {
                    // 게임 클리어되면 UI를 보여준다.
                    ABPlayerController->ShowResultUI();
                }
            }
        }
    }
    ...
    ABCharacter.cpp
    ...
    void AABCharactter::SetCharacterState(ECharacterState NewState)
    {
        ...
        // DEAD state : 캐릭터가 HP를 소진해 사망할 때 발생하는 state
        case ECharacterState::DEAD:
        {
            ...
            GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
                if (bIsPlayer)
                {
                    // 플레이어가 사망하면 게임 재시작
                    //ABPlayerController->RestartLevel();

                    // 게임 종료 UI 띄우기
                    ABPlayerController->ShowResultUI();
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
        ...
    }

    ABGameplayWidget의 btnRetryGame 버튼 기능을 구현하고
    미리 UI 위젯이 GameState의 정보를 읽도록 바인딩 해준다.
    ABGameplayResultWidget.h
    ...
    public:
        // NativeContruct 함수는 AddToViewport 함수가 외부에서 호출될 떄 UI 위젯이 초기화되며 호출된다.
        // 그래서 PlayerController에서 ShowResultUI 안에 AddToViewport 함수 호출 전
        // 미리 UI 위젯이 GameState 정보를 읽도록 바인딩한다.
        void BindGameState(class AABGameState* GameState);
    private:
	    TWeakObjectPtr<class AABGameState> CurrentGameState;
    ...
    ABGameplayResultWidget.cpp
    ...
    void UABGameplayResultWidget::BindGameState(AABGameState* GameState)
    {
        ABCHECK(nullptr != GameState);
        CurrentGameState = GameState;
    }

    void UABGameplayResultWidget::NativeConstruct()
    {
        Super::NativeConstruct();

        ABCHECK(CurrentGameState.IsValid());

        auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult")));
        ABCHECK(nullptr != Result);
        Result->SetText(FText::FromString(CurrentGameState->IsGameCleared() ?
            TEXT("Mission Complete") : TEXT("Mission failed")));

        auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore")));
        ABCHECK(nullptr != TotalScore);
        TotalScore->SetText(FText::FromString(FString::FromInt(CurrentGameState->GetTotalGameScore())));
    }
    ...
    ABGameplatWidget.cpp
    ...
    void UABGameplayWidget::OnRetryGameClicked()
    {
        // 자신을 생성하고 관리하는 Controller 정보를 가져온다.
        auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer());
        ABCHECK(nullptr != ABPlayerController);
        ABPlayerController->RestartLevel();
    }
    ...
    ABPlayerController.cpp
    ...
    void AABPlayerController::ShowResultUI()
    {
        auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
        ABCHECK(nullptr != ABGameState);
        ResultWidget->BindGameState(ABGameState);

        ResultWidget->AddToViewport();
        ChangeInputMode(false);
    }


*/