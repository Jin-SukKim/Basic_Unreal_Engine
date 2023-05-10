/*
    
    액터 설정을 완료하고 액터의 로직을 state machine으로 설계한다.
    
    1. Ready state : 액터의 시작 스테이트
        문을 열어놓고 대기하닥 중앙의 박스 트리거로 플레이어의 진입을 감지하면 전투 스테이트로 이동한다.

    2. Battle state : 문을 닫고 일정 시간이 지나면 NPC를 소환한다.
        일정 시간이 지나면 랜덤한 위치에 아이템 상자도 생성한다.
        소환한 NPC가 죽으면 완료 스테이트로 이동한다.

    3. Complete state : 닫힌 문을 연다. 
        각 문마다 배치한 트리거 게이트로 플레이어를 감지하면 이동한 문의 방향으로 새로운 섹션을 소환한다.

    state machine의 기획을 한 후 enum을 사용해 구현해본다.

    추가로 고려할 부분은 캐릭터가 시작하는 섹션은 전투 없이 완료 스테이트에서 시작할 수 있도록
    NoBattle이라는 속성을 액터에 추가하는 점이다.
    에디터에서 배치한 레벨에 이값을 체크하면 해당 액터는 전투없이 캐릭터가 다음 섹션으로 통과할 수 있게 설정된다.

    NPC와 아이템 상자의 생성 기능을 제외하고 각 state별로 구현한 기능
    ABSection.h
    ...
    // state machine
	enum class ESectionState : uint8
	{
		READY = 0,
		BATTLE,
		COMPLETE
	};

	void SetState(ESectionState NewState);
	ESectionState CurrentState = ESectionState::READY;

	void OperateGates(bool bOpen = true);

	UPROPERTY(EditAnywhere, Category = State, Meta = (AllowPrivateAccess = true))
	bool bNoBattle;
    ...
    ABSection.cpp
    ... 생산자.
    bNoBattle = false;
    ...
    // Called when the game starts or when spawned
    void AABSection::BeginPlay()
    {
        Super::BeginPlay();
        
        SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
    }
    void AABSection::SetState(ESectionState NewState)
    {
        switch (NewState)
        {
        case ESectionState::READY:
        {
            // collision preset 설정
            Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
            for (UBoxComponent* GateTrigger : GateTriggers)
            {
                GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
            }

            OperateGates(true);
            break;
        }
        case ESectionState::BATTLE:
        {
            Trigger->SetCollisionProfileName(TEXT("NoCollision"));
            for (UBoxComponent* GateTrigger : GateTriggers)
            {
                GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
            }

            OperateGates(false);
            break;
        }
        case ESectionState::COMPLETE:
        {
            Trigger->SetCollisionProfileName(TEXT("NoCollision"));
            for (UBoxComponent* GateTrigger : GateTriggers)
            {
                GateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
            }

            OperateGates(true);
            break;
        }
        }

        CurrentState = NewState;
    }

    void AABSection::OperateGates(bool bOpen)
    {
        for (UStaticMeshComponent* Gate : GateMeshes)
        {	// 다음 섹션으로 넘어갈 수 있으면 게이트를 회전시켜 문이 열린것 처럼 표현한다.
            Gate->SetRelativeRotation(bOpen ? FRotator(0.f, -90.f, 0.f) : FRotator::ZeroRotator);
        }
    }

    이러면 NoBattle 속성을 true로 설정하고 플레이시 문이 열린다.
    하지만 제작 단계에서 완료 스테이트 상황으로 몯느 문이 열리도록 제작자에게
    보여지면 더 편리할 것이다.

    Actor에는 에디터와 연동된느 OnContruction이라는 특별한 함수가 설계돼 있다.
    에디터 작업에서 선택한 액터의 속성이나 트랜스폼 정보가 변경될 때 이 OnConstruction 함수가 실행된다.

    OnConstruction 함수에 액터와 컴포넌트의 속성을 설정하면 작업 중인 레벨에서도 미리 결과를 확인할 수 있따.
    ABSection.h
    ...
    // 에디터와 연동되는 함수로 에디터 작업을 하면 플레이하지 않아도 변경사항을 볼 수 있다.
	virtual void OnConstruction(const FTransform& Transform) override;
    ...
    ABSection.cpp
    ...
    void AABSection::OnConstruction(const FTransform& Transform)
    {
        Super::OnConstruction(Transform);
        SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
    }
    ...

    액터의 완료 스테이트에서는 각 철문에 있는 트리거가 활성화되고, 플레이어가 감지되면 해당 철문의 방향으로
    새로운 섹션 액터를 생성해야된다.
    게임의 진행 상황에 따라 해당 위치에 이미 섹션 액터가 생성돼 있을 수 있으므로 이를 미리 확인하는 로직이 필요하다.
    물리 엔진 기능을 사용해 해당 위치에 액터가 있다면 생성을 건너뛴다.

    생성된 액터는 READY state에서 시작하는데 가운데 트리거 영역을 활성화해서 플레이어가 들어오는지 감지해야 한다.
    플레이어를 감지하면 BATTLE state로 전환하고 문을 닫는다.

    각 박스 컴포넌트의 감지 기능을 사용해 위의 내용을 구현한다.
    BoxComponent의 OnComponentBeginOverlap delegate에 바인딩시킬 함수를 생성하고 연결한다.
    해당 delegate는 dynamic delegate이므로 함수 선언에 UFUNCTION을 지정한다.

    4개의 문은 기능이 동일하므로 모든 문의 박스 컴포넌트의 delegate에 하나의 멤버 함수를 연결한다.
    이때 감지된 boxComponent가 어떤 문에 있는 Component인지 구분할 수 있도록 컴포넌트에 socket이르므올
    Tag를 설정하고, 이를 사용해 해당 방향에 띄울 다음 섹션 액터까지 생성하는 기능을 구현한다.
    ABSection.h
    ...
    // Gate를 넘어가면 섹션을 생성하거나 이미 생성된 섹션이 있는지 확인하기 위해 Delegate 설정
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    ...
    ABSection.cpp
    ...생산자
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap);
    ...
    NewGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap);
	// 어떤 게이트에 감지됬는지 구분하기위해 Tag 설정
	NewGateTrigger->ComponentTags.Add(GateSocket);
    ...
    void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
    {
        if (CurrentState == ESectionState::READY)
        {
            SetState(ESectionState::BATTLE);
        }
    }

    void AABSection::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
    {
        ABCHECK(OverlappedComponent->ComponentTags.Num() == 1);
        
        // 게이트에 붙인 Tag 확인
        FName ComponentTag = OverlappedComponent->ComponentTags[0];
        FName SocketName = FName(*ComponentTag.ToString().Left(2));
        if (!Mesh->DoesSocketExist(SocketName))
            return;

        // Collision에서 Overlap이 감지된 게이트의 위치
        FVector NewLocation = Mesh->GetSocketLocation(SocketName);

        TArray<FOverlapResult> OverlapResults;
        // Collision 반응 설정
        FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);
        // 찾는 오브젝트 타입
        FCollisionObjectQueryParams ObjectQueryParam(FCollisionObjectQueryParams::InitType::AllObjects);
        bool bResult = GetWorld()->OverlapMultiByObjectType(
            OverlapResults,
            NewLocation,
            FQuat::Identity,
            ObjectQueryParam,
            FCollisionShape::MakeSphere(775.f),
            CollisionQueryParam
        );

        if (!bResult)
        {
            auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator);
        }
        else
        {
            ABLOG(Warning, TEXT("New section area is not empty."));
        }
    }

    이제 게이트 트리거에 플레이어가 감지되면 해당 뱡향으로 새로운 섹션 액터가 생성되고
    새롭게 생성된 섹션 액터 안으로 집입하면 문이 닫힌다.

*/