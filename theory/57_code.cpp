/*
    ABUIPlayerController.h
    ...
    protected:
        virtual void BeginPlay() override;

        UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
        TSubclassOf<class UUserWidget> UIWidgetClass;

        UPROPERTY()
        class UUSerWidget* UIWidgetInstance;
    ...
    ABUIPlayerController.cpp
    ...
    #include "Blueprint/UserWidget.h"

    void AABUIPlayerController::BeginPlay()
    {
        Super::BeginPlay();

        ABCHECK(nullptr != UIWidgetClass);

        // UI 인스턴스 생성
        UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
        ABCHECK(nullptr != UIWidgetInstance);

        // 뷰포트에 UI 인스턴스 띄우기
        UIWidgetInstance->AddToViewport();

        // 입력은 UI에만 전달되도록 한다.
        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
        SetInputMode(Mode);
        bShowMouseCursor = true;
    }

    현재 Level에 있는 스켈레탈 메시 액터의 목록을 가져오고 버튼을 누를 때마다
    Skeletal Mesh Component에 지정한 메시를 변경하는 기능을 구현해준다.
    ABCharacterSleectWidget.h
    ...
    protected:
        UFUNCTION(BlueprintCallable)
        void NextCharacter(bool bForward = true);

        virtual void NativeConstruct() override;

    protected:
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
        int32 CurrentIndex;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
        int32 MaxIndex;

        TWeakObjectPtr<USkeletalMeshComponent> TargetComponent;
    ...
    ABCharacterSelectWidget.cpp
    ...
    #include "ABCharacterSetting.h"
    #include "ABGameInstance.h"
    #include "EngineUtils.h""
    #include "Animation/SkeletalMeshActor.h""

    void UABCharacterSelectWidget::NextCharacter(bool bForward)
    {
        bForward ? CurrentIndex++ : CurrentIndex--;

        // 무한반복
        if (CurrentIndex == -1) CurrentIndex = MaxIndex - 1;
        if (CurrentIndex == MaxIndex) CurrentIndex = 0;

        // Skeletal Mesh 목록에서 애셋 Reference 가져오기
        auto CharacterSetting = GetDefault<UABCharacterSetting>();
        auto AssetRef = CharacterSetting->CharacterAssets[CurrentIndex];

        auto ABGameInstance = GetWorld()->GetGameInstance<UABGameInstance>();
        ABCHECK(nullptr != ABGameInstance);
        ABCHECK(TargetComponent.IsValid());

        // 게임 진행중에도 비동기 방식으로 애셋을 로딩
        USkeletalMesh* Asset = ABGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef);
        if (nullptr != Asset)
        {
            TargetComponent->SetSkeletalMesh(Asset);
        }
    }

    void UABCharacterSelectWidget::NativeConstruct()
    {
        Super::NativeConstruct();

        CurrentIndex = 0;
        auto CharacterSetting = GetDefault<UABCharacterSetting>();
        MaxIndex = CharacterSetting->CharacterAssets.Num();

        for (TActorIterator<ASkeletalMeshActor> It(GetWorld()); It; ++It)
        {
            TargetComponent = It->GetSkeletalMeshComponent();
            break;
        }
    }
    ...

    버튼 Control과 NextCharacter 함수를 직접 바인딩
    ABCharacterSelectWidget.h
    ...
        UPROPERTY()
        class UButton* PrevButton;

        UPROPERTY()
        class UButton* NextButton;

        UPROPERTY()
        class UEditableTextBox* TextBox;

        UPROPERTY()
        class UButton* ConfirmButton;

    private:
        UFUNCTION()
        void OnPrevClicked();

        UFUNCTION()
        void OnNextClicked();

        UFUNCTION()
        void OnConfirmClicked();
    ...
    ABCharacterSelectWidget.cpp
    ...
    #include "EngineUtils.h"
    #include "Animation/SkeletalMeshActor.h"
    #include "Components/Button.h"
    #include "Components/EditableTextBox.h"
    #include "ABSaveGame.h"
    #include "ABPlayerState.h"


    void UABCharacterSelectWidget::NextCharacter(bool bForward)
    {
        // bForward가 true면 앞으로 false면 뒤로
        bForward ? CurrentIndex++ : CurrentIndex--;

        // 무한반복
        if (CurrentIndex == -1) CurrentIndex = MaxIndex - 1;
        if (CurrentIndex == MaxIndex) CurrentIndex = 0;

        // Skeletal Mesh 목록에서 애셋 Reference 가져오기
        auto CharacterSetting = GetDefault<UABCharacterSetting>();
        auto AssetRef = CharacterSetting->CharacterAssets[CurrentIndex];

        auto ABGameInstance = GetWorld()->GetGameInstance<UABGameInstance>();
        ABCHECK(nullptr != ABGameInstance);
        ABCHECK(TargetComponent.IsValid());

        // 게임 진행중에도 비동기 방식으로 애셋을 로딩
        USkeletalMesh* Asset = ABGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef);
        if (nullptr != Asset)
        {
            TargetComponent->SetSkeletalMesh(Asset);
        }
    }

    void UABCharacterSelectWidget::NativeConstruct()
    {
        Super::NativeConstruct();

        CurrentIndex = 0;
        auto CharacterSetting = GetDefault<UABCharacterSetting>();
        MaxIndex = CharacterSetting->CharacterAssets.Num();

        for (TActorIterator<ASkeletalMeshActor> It(GetWorld()); It; ++It)
        {
            TargetComponent = It->GetSkeletalMeshComponent();
            break;
        }

        // Button Control
        PrevButton = Cast<UButton>(GetWidgetFromName(TEXT("btnPrev")));
        ABCHECK(nullptr != PrevButton);

        NextButton = Cast<UButton>(GetWidgetFromName(TEXT("btnNext")));
        ABCHECK(nullptr != NextButton);

        TextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("edtPlayerName")));
        ABCHECK(nullptr != TextBox);

        ConfirmButton = Cast<UButton>(GetWidgetFromName(TEXT("btnConfirm")));
        ABCHECK(nullptr != ConfirmButton);

        // Button Control과 함수들 Binding
        PrevButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnPrevClicked);
        NextButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnNextClicked);
        ConfirmButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnConfirmClicked);
    }

    void UABCharacterSelectWidget::OnPrevClicked()
    {
        NextCharacter(false);
    }

    void UABCharacterSelectWidget::OnNextClicked()
    {
        NextCharacter(true);
    }

    void UABCharacterSelectWidget::OnConfirmClicked()
    {
        FString CharacterName = TextBox->GetText().ToString();
        if (CharacterName.Len() <= 0 || CharacterName.Len() > 10) return;

        // save data unreal object
        UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
        NewPlayerData->PlayerName = CharacterName;
        NewPlayerData->Level = 1;
        NewPlayerData->Exp = 0;
        NewPlayerData->HighScore = 0;

        auto ABPlayerState = GetDefault<AABPlayerState>();
        if (UGameplayStatics::SaveGameToSlot(NewPlayerData, ABPlayerState->SaveSlotName, 0))
        {
            // Level 열기
            UGameplayStatics::OpenLevel(GetWorld(), TEXT("Gameplay"));
        }
        else
        {
            ABLOG(Error, TEXT("SaveFame Error!"));
        }

    }

    캐릭터 생성 버튼을 누르면 현재 선택한 캐릭터 정보를 세이브 데이터에 저장하고 
    이를 읽어들이는 기능
    ABSaveGame.h
    ...
    UPROPERTY()
	int32 CharacterIndex;
    ...
    ABSaveGame.cpp
    ... 생성자
    CharacterIndex = 0;
    ...
    ABCharacterSelectWdiget.cpp
    ...
    void UABCharacterSelectWidget::OnConfirmClicked()
    {
        ...
        // save data unreal object
        UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
        NewPlayerData->PlayerName = CharacterName;
        NewPlayerData->Level = 1;
        NewPlayerData->Exp = 0;
        NewPlayerData->HighScore = 0;
        NewPlayerData->CharacterIndex = CurrentIndex;
        ...
    }
    ABPlayerState.h
    ...
    int32 GetCharacterIndex() const;
    ...
    UPROPERTY(Transient)
	int32 CharacterIndex;
    ...
    ABPlayerState.cpp
    ...생성자
    CharacterIndex = 0;
    ...
    int32 AABPlayerState::GetCharacterIndex() const
    {
        return CharacterIndex;
    }
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
        Exp = ABSaveGame->Exp;
        CharacterIndex = ABSaveGame->CharacterIndex;
        // 최초에 플레이어 데이터 생성하자마자 바로 저장
        SavePlayerData();
    }
    ...
    ABCharacter.cpp
    ...
    void AABCharactter::BeginPlay()
    {
        ...
        // ArenaBattleSetting에서 INI 파일의 애셋 경로를 읽어온다.
        auto DefaultSetting = GetDefault<UABCharacterSetting>();

        if (bIsPlayer)
        {
            auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
            ABCHECK(nullptr != ABPlayerState);
            AssetIndex = ABPlayerState->GetCharacterIndex();
        }
        else
        {
            // 애셋 목록에서 경로들 중 하나 랜덤하게 가져오기
            AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
        }
        ...
    }
    ...
*/