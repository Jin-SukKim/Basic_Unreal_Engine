/*
    액터 컴포넌트의 제작

    액터 컴포넌트 클래스를 생성하고 캐릭터에 부착해 스탯 관리를 하도록 한다.
    캐릭터 클래스에 생성한 컴포넌트를 멤버 변수로 선언해 부착해준다.
    Character.h
    ...
    // 캐릭터 스탯 관리용 액터 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UABCharacterStatComponent* CharacterStat;
    ...
    .cpp
    CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));

    액터 컴포넌트를 생성하면 자동으로 제공되는 템플릿 코드에 BeginPlay와 TickComponent 함수가 제공된다.
    현재 프로젝트는 스택에 변경이 일어날 때만 데이터를 처리할 예정이다 Tick 로직이 필요없다.

    따라서 액터 컴포넌트의 설정을 이에 맞게 변경해준다.
    액터의 PostInitializeComponents에 대응하는 컴포넌트 함수는 InitializeComponent 함수다.
    이 함수는 엑터의 PostInitializeComponents 함수가 호출되기 직전에 호출된다.

    이 함수르 사용해 컴포넌트의 초기화 로직을 구현해주는데, 호출되려면 생성자에서
    bWantsInitializeComponent값을 true로 설정해줘야 한다.

    StatComponent.cpp
    // Sets default values for this component's properties
    UABCharacterStatComponent::UABCharacterStatComponent()
    {
        // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
        // off to improve performance if you don't need them.
        PrimaryComponentTick.bCanEverTick = false;	// tick 함수를 사용하지 않아서 false로 설정
        // InitializeComponent 함수를 사용하기 위해 이 값을 true로 설정해준다.
        bWantsInitializeComponent = true;

        // ...
    }

    // 초기화 함수
    void UABCharacterStatComponent::InitializeComponent()
    {
        Super::InitializeComponent();
    }

    모든 스탯을 스탯 컴포넌트에서 관리하도록 변수를 추가로 선언한다.
    데이터를 관리하는 변수들은 private으로 한정해 선언하고 레벨은 SetNewLevel 함수를
    통해서만 변경할 수 있도록 설계한다.

    Game Instance에서 데이터를 가져와 초기화하고 레벨이 변경되면 해당 스탯이 바뀌도록
    제작한다.

    StatComponent.h
    ...
    public:	
        // 레벨 변경
        void SetNewLevel(int32 NewLevel);

    private:
        // 데이터를 관리하는 변수들
        struct FABCharacterData* CurrentStatData = nullptr;

        UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
        int32 Level;

        UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
        float CurrentHP;
	StatComponent.cpp
    ... 생성자
    Level = 1;
    ...
    // 초기화 함수
    void UABCharacterStatComponent::InitializeComponent()
    {
        Super::InitializeComponent();

        // 처음 스탯 데이터 설정
        SetNewLevel(Level);
    }

    void UABCharacterStatComponent::SetNewLevel(int32 NewLevel)
    {
        // 데이터를 가져오기 위해 Game Instance 캐스트
        auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

        // ABGameInstance가 맞는지 확인
        ABCHECK(nullptr != ABGameInstance);
        // GetABCharacterData 함수를 사용해 CSV 데이터 가져오기
        CurrentStatData = ABGameInstance->GetABCHaracterData(NewLevel);
        if (nullptr != CurrentStatData)
        {
            Level = NewLevel;
            CurrentHP = CurrentStatData->MaxHP;
        }
        else
        {
            ABLOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
        }
    }

    언리얼 오브젝트에는 직렬화(Serialization) 기능이 있어 오브젝트의
    UPROPERTY 속성을 저장하고 로딩할 수 있다.
    하지만 컴포넌트의 스탯 중 CurrentHP 값은 게임을 시작할 때마다 변경되므로
    이 값을 보고나하는 것은 의미가 없고 오히려 오브젝트 저장 시 디스크 공간만 낭비한다
    이런 속석에는 Transient 키워드를 추가해 해당 속성을 직력화에서 제외시키는 것이 좋다.

    이제 대미지를 받으면 CurrentHP에서 차감하고 결과값이 0보다 작거나 같으면 죽는 기능을 추가한다.

    지금까지 대미지는 캐릭터의 TakeDamage 함수에서 직접 처리했는데 StatComponent 클래스에
    SetDamage 함수를 생성하고 TakeDamage 함수에서 이를 호출해 대미지를 액터 컴포넌트가
    처리하도록 구성을 변경한다.

    액터 컴포넌트는 대미지 계산을 처리하되 CurrnetHP 값이 소진되면 죽었다고 캐릭터에게 알려준다.

    액터 컴포넌트가 캐릭터에 의존성을 가지지 않도록, Actor Component에 델리게이트를 선언하고
    캐릭터에서 이를 바인딩시키는 형태로 구조를 설계한다.
    StatComponent.h
    DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);
    ...
	// 스탯 컴포넌트에서 대미지 처리
	void SetDamage(float NewDamage);
	float GetAttack();
	FOnHPIsZeroDelegate OnHPIsZero;
    ...
    StatComponent.cpp
    ...
    void UABCharacterStatComponent::SetDamage(float NewDamage)
    {
        ABCHECK(nullptr != CurrentStatData);
        // 대미지입은 HP 계산
        CurrentHP = FMath::Clamp<float>(CurrentHP - NewDamage, 0.f, CurrentStatData->MaxHP);
        if (CurrentHP <= 0.f)
        {	
            // HP가 0 이하면 죽음을 알려준다.
            OnHPIsZero.Broadcast();
        }
    }

    float UABCharacterStatComponent::GetAttack()
    {
        ABCHECK(nullptr != CurrentStatData, 0.f);
        return CurrentStatData->Attack;
    }
    ...
    Character.cpp
    ...
    void AABCharacter::PostInitializeComponents()
    {
        ...
        // 공격을 체크
        ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharactter::AttackCheck);

        // 대미지를 입어 HP가 0이하면 죽는 애니메이션 재생 및 충돌 금지
        CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
            ABLOG(Warning, TEXT("OnHPIsZero"));
            ABAnim->SetDeadAnim();
            SetActorEnableCollision(false);
        });
    }

    float AABCharactter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
    {
        float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
        ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
        // StatComponent 클래스를 사용해 대미지 계산
        CharacterStat->SetDamage(FinalDamage);
        return FinalDamage;
    }

    void AABCharacter::AttackCheck() 
    {
        ...
        // 레벨마다 주는 대미지가 바뀌므로 StatComponent에서 대미지 값을 가져온다.
        HitResult.GetActor()->TakeDamage(CharacterStat->GetAttack(), DamageEvent, GetController(), this);
        ...
    }
*/