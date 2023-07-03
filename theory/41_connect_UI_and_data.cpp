/*
    UI와 데이터의 연동

    UI와 데이터를 연동해 프로그래스바가 변경되도록 한다.
    
    UI의 작업은 Anim Graph와 유사하게 디자이너 공간에서 진행했고
    UI의 로직은 Anim Instance와 유사하게 C++ 클래스에서 제공할 수 있다.
    Widget Blueprint가 사용하는 기반 C++ 클래스는 UserWidget이다.

    UserWidget을 상속받은 클래스를 생성해준다.
    그리고 StatComponent와 연동해 스탯이 변경될 때마다 프로그래스바의 내용을 업데이트 한다.
    이번에도 상호 의존성을 가지지 않게 Delegate를 사용한다.

    StatComponent.h
    DECLARE_MULTICAST_DELEGATE(FOnHPChangeDelegate);
    ...
    // HP 변화
	void SetHP(float NewHP);
	float GetHPRatio();
	FOnHPChangeDelegate OnHPChanged;
    ...
    StatComponent.cpp
    ...
    void UABCharacterStatComponent::SetNewLevel(int32 NewLevel)
    {
        ...
        if (nullptr != CurrentStatData)
        {
            Level = NewLevel;
            SetHP(CurrentStatData->MaxHP);
            CurrentHP = CurrentStatData->MaxHP;
        }
        ...
    }

    void UABCharacterStatComponent::SetDamage(float NewDamage)
    {
        ABCHECK(nullptr != CurrentStatData);
        // UI 위젯과 연동을 위한 새로운 대미지 계산 함수
        SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.f, CurrentStatData->MaxHP));
    }

    float UABCharacterStatComponent::GetAttack()
    {
        ABCHECK(nullptr != CurrentStatData, 0.f);
        return CurrentStatData->Attack;
    }

    void UABCharacterStatComponent::SetHP(float NewHP)
    {
        CurrentHP = NewHP;
        OnHPChanged.Broadcast();
        // float의 값을 0과 비교시 미세한 오차 범위 내에 있는지 보고 판단하는 것이 좋다.
	    // UE는 무시 가능한 오차를 측정할 때 사용하도록 KINDA_SMALL_NUMBER 매크로를 제공한다.
        if (CurrentHP < KINDA_SMALL_NUMBER)
        {
            // HP가 0 이하 근처면 죽음을 알려준다.
            CurrentHP = 0.f;
            OnHPIsZero.Broadcast();
        }
    }

    float UABCharacterStatComponent::GetHPRatio()
    {
        ABCHECK(nullptr != CurrentStatData, 0.f);
        return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.f : (CurrentHP / CurrentStatData->MaxHP);
    }

    StatComponent의 Delegate 로직을 완성하면 UI에서 캐릭터 컴포넌트에 연결해 HP가 변할떄 
    ProgressBar를 업데이트 해주는 기능을 추가한다.

    이번에는 약 포인터를 사용해 봤다.
    언리얼 옵젝트의 약 포인터 선언은 TWeakObjectPtr을 사용한다.

    현재는 캐릭터와 UI 위젯이 생사를 같이하기에 약 포인터가 필요 없지만 학습을 위해 사용해봤다.

    만약 UI와 캐릭터가 서로 다른 Actor라면 약 포인터를 사용하는 것이 좋다.

    CharacterWidget.h
    ...
    public:
	void BindCharacterStat(class UABCharacterStatComponent* NewCharacterStat);
    private:
        TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat;
    ...
    CharacterWidget.cpp
    #include "ABCharacterStatComponent.h"

    void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat)
    {
        ABCHECK(nullptr != NewCharacterStat);

        CurrentCharacterStat = NewCharacterStat;
        NewCharacterStat->OnHPChanged.AddLambda([this]() -> void {
            if (CurrentCharacterStat.IsValid())
            {
                ABLOG(Warning, TEXT("HPRatio : %f"), CurrentCharacterStat->GetHPRatio());
            }
        });
    }

    캐릭터의 BeginPlay 함수에서 캐릭터 컴포넌트와 UI 위젯을 연결한다.
    Character.cpp
    ...
    // Called when the game starts or when spawned
    void AABCharactter::BeginPlay()
    {
        ...
        // 위젯의 초기화는 BeinPlaty에서 초기화 된다.
        // UI 위젯을 캐릭터의 StatComponenet와 연동
        auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
        if (nullptr != CharacterWidget)
        {
            CharacterWidget->BindCharacterStat(CharacterStat);
        }
    }

    코드를 작성해준 뒤 위젯 블루프린트에서 제작한 클래스를 상속받도록 설정한다.

    UI 툴 우측 상단의 Graph로 이동한 뒤 툴바의 Class Setting에서 Parenct class를 생성한 클래스로 변경해준다.

    연동이 잘 됬으면 HPRatio를 사용해 ProgressBar 위젯의 값을 업데이트 한다.
    생성한 위젯을 검색하고 해당 위젯의 속성을 업데이트하는 로직을 추가한다.

    한가지 고려사항은 UI가 초기화되는 시점이다.
    UI 시스템이 준비되면 NAtiveConstruct 함수가 호출되는데, UI 생성은 플레이어 컨트롤러의 BeginPlay에서 호출된다.
    NativeConstruct 함수에서 위젯 내용을 업데이트 하는 로직을 구현한다.

    CharacterWidget.h
    ...
    protected:
        virtual void NativeConstruct() override;
        void UpdateHPWidget();
    private:
        UPROPERTY()
	    class UProgressBar* HPProgressBar;
    ...
    CharacterWidget.cpp
    
    void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat)
    {
        ABCHECK(nullptr != NewCharacterStat);

        CurrentCharacterStat = NewCharacterStat;
        // StatComponent와 연동되는 Delegate
        NewCharacterStat->OnHPChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget);
    }

    // UI 시스템이 준비되면 호출되는 함수
    void UABCharacterWidget::NativeConstruct()
    {
        Super::NativeConstruct();
        // 생성한 위젯 이름으로 위젯을 검색
        HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
        ABCHECK(nullptr != HPProgressBar);
        // 위젯 속성을 업데이트
        UpdateHPWidget();
    }

    void UABCharacterWidget::UpdateHPWidget()
    {
        if (CurrentCharacterStat.IsValid())
        {
            if (nullptr != HPProgressBar)
            {	
                // 스탯에서 HPRatio를 가져와 속성 업데이트
                HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
            }
        }
    }

*/