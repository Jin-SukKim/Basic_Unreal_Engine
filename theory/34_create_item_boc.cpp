/*
    아이템 상자의 제작

    Actor를 부모로하는 ABItemBox 클래스를 생성한다.

    아이템 상자는 플레이어를 감지하는 콜리전 박스와 아이템 상자를 시각화해주는 스태틱메시로 나뉜다.
    루트 컴포넌트에는 플레이어와의 겹침(Overlap)을 감지할 박스 콜리전 컴포넌트를 사용하고,
    자식에는 스태틱메시 컴포넌트를 추가한다.

    원하는 박스 애셋을 선택해 열어보면 왼쪽 상당에서 메시의 대략적인 크기를 볼 수 있다.
    사용하고 있는 애셋의 크기는 53x55x27(cm단위)이다.

    원본 애셋의 크기가 너무 작아 크게 만들고 싶다면 LODO 섹션에 있는 빌드 스케일 옵션을 설정하면 된다.
    원본 데이터의 스케일을 변경하면 게임 로직에서 트랜스폼의 스케일 값을 신경쓰지 않아도 되므로 효과적이다.

    애셋의 크기를 참고해 액터에 박스 콜리전 컴포넌트의 크기와 스태틱메시 컴포넌트의 애셋을 지정한다.
    박스 콜리전 컴포넌트의 Extend 값은 전체 박스 영역 크기의 절반 값을 의미한다.

    ABItemBox.h
    ...
	UPROPERTY(VisibleAnywhere, Catergory = Box)
	UBoxComponent* Trigger;
	
	UPROPERTY(VisibleAnywhere, Catergory = Box)
	USkeletalMeshComponent* Box;
    ...
    ABItemBox.cpp
    ...
    // Sets default values
    AABItemBox::AABItemBox()
    {
        // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
        PrimaryActorTick.bCanEverTick = false;

        Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
        Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));


        RootComponent = Trigger;
        Box->SetupAttachment(RootComponent);

        // 박스 콜리전 컴포넌트의 Extend 값은 전체 박스 영역 크기의 절반 값이다.
        Trigger->SetBoxExtent(FVector(40.f, 42.f, 30.f));
        static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1"));
        if (SM_BOX.Succeeded())
        {
            Box->SetStaticMesh(SM_BOX.Object);
        }

        Box->SetRelativeLocation(FVector(0.f, -3.5f, -30.f));

    }
    ...

    폰이 아이템을 획득하도록 기본 반응은 Ignore이라 하는 오브젝트 채널을 생성한다.
    그리고 동일한 이름의 프리셋도 하나 만들어준다.
    Collision은 Query Only(No Physics Collision)으로 지정해 감지만 하도록 한다.
    그리고 이 오브젝트 채널은 ABCharacter 오브젝트 채널에만 반응하도록 Overlap 세팅을 지정하고 나머지는 Ignore한다.
    ABCharacter의 프리셋도 ItemBox 콜리전과 Overlap하게 바꾼다.

    새로운 프리셋을 박스 컴포넌트에 설정하고, 박스 컴포넌트에서 캐릭터를 감지할 때 관련된 행동을 구현한다.
    박스 컴포넌트에는 Overlap 이벤트를 처리할 수 있게 OnComponentBeginOverlap이라는 데리게이트가 선언되있따.
    해당 델리게이트 선언

    UPROPERTY(BlueprintAssignable, Category = "Collision")
    FComponentBeginoverlapsSignature OnComponentBeginOverlap

    델리게이트를 선언하는 데 사용한 FComponentBeginOVerlapSignature는 UE에 다음과 같이 선언돼있다.

    PrimitiveComponent.h
    ...
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(
        FComponentBeginOverlapSignater, UPrimitiveComponent*, OverlappedComponent, AACtor*. OtherActor,
        UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex, bool, bFromSweep, const FHitResult&, SweepResult
    );

    매크로를 보면 OnComponentHBeginOverlap 델리게이트는 멀티캐스트 다이내믹 델리게이트임을 알 수 있다.
    유형과 인자를 모두 복사해 매크로 설정과 동일한 멤버 함수를 선언하고
    이를 해당 델리게이트에 바인딩하면 Overlap 이벤트가 발생할 때마다 바인딩한 멤버 함수가 호출된다.

    즉, 박스와 캐릭터는 서로 Overlap이 가능하고, 추후 둘이 충돌했을 때 onComponentOverlap 함수를 통해 무기를 장착하도록 구현한다.
    ABItemBox.h
    ...
    virtual void PostInitializeComponents() override;
    ...
    // 충돌 시 호출되는 함수
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherOBdyIndex, bool bFromSweep, const FHitResult& SweepResult);
    ...
    ABItemBox.cpp
    ...
    // 오브젝트 콜리전 채널
	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	// 스태틱 메시 충돌 설정
	Box->SetCollisionProfileName(TEXT("NoCollision"));
    ...
    void AABItemBox::PostInitializeComponents()
    {
        Super::PostInitializeComponents();
        Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnCharacterOverlap);
    }

    void AABItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherOBdyIndex, bool bFromSweep, const FHitResult& SweepResult)
    {
        ABLOG_S(Warning);
    }

    캐릭터가 아이템 박스를 통과할 때마다 오버랩 델리게이트와 바인딩된 함수가 실행된다.

*/