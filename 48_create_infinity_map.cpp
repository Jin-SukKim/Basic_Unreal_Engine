/*
    무한맵 생성

    레벨을 섹션이라는 단위로 나누고 하나의 섹션을 클리어하면 새로운 섹션이 등장하는
    무한 맵 스테이지를 제작해본다.

    섹션 액터가 해야할 일

    1. 섹션의 배경과 네 방향으로 캐릭터 입장을 통제하는 문을 제공
    2. 플레이어가 섹션에 진입하면 모든 문을 닫는다.
    3. 문을 닫고 일정 시간 훙 ㅔ섹션 중아에서 NPC를 생성한다.
    4. 문을 닫고 일정 시간 후에 아이템 상자를 섹션 내 랜덤한 위치에 생성한다.
    5. 생성한 NPC가 죽으면 모든 문을 개방한다.
    6. 통과한 문으로 이어지는 새로운 섹션을 생성한다.

    섹션 액터 제작을 위해 Actor를 부모 클래스로 하는 ABSection 클래스를 ArenaBattle 모듈에 생성해준다.
    이 섹션은 주요 배경으로 SM_SQUARE 애셋을 사용한다.
    액터에 스태틱메시 컴포넌트를 선언하고 루트로 설정한 뒤 SM_SQUARE 애셋을 지정한다.
    ABSection.h
    ...
    private:
        UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = ture))
        UStaticMeshComponent* Mesh;
    ...
    ABSection.cpp
    ...
    AABSection::AABSection()
    {
        // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
        PrimaryActorTick.bCanEverTick = false;

        Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
        RootComponent = Mesh;

        FString AssetPath = TEXT("/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE");
        static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SQUARE(*AssetPath);
        if (SM_SQUARE.Succeeded())
        {
            Mesh->SetStaticMesh(SM_SQUARE.Object);
        }
        else
        {
            ABLOG(Error, TEXT("Failed to load staticmesh asset : %s"), *AssetPath);
        }

    }
    ...

    이 스태틱메시 애셋은 각 방향별로 출입문과 섹션을 이어붙일 수 있게 여덜 개의 소켓이 부착돼 있따.
    배경의 각 출입구에 철문 애셋을 부착한다.
    철문 애셋은 pivot이 왼쪽에 있으므로 부착하는 최종 위치는 소켓 위치로부터 y축 -80.5만큼 이동한 지점이다.
    소켓 목록을 제작하고 이를 사용해 철문을 각각 부착한다.
    각각의 철문은 동일한 기능을 가지므로 TArray로 묶어 관리한다.
    ABSection.h
    ...
    // 소켓에 부착될 철문들
	UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = true))
	TArray<UStaticMeshComponent*> GateMeshes;
    ...
    ABSection.cpp
    ... 생성자
    FString GateAssetPath = TEXT("/Game/Book/StaticMesh/SM_GATE.SM_GATE");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GATE(*GateAssetPath);
	if (!SM_GATE.Succeeded())
	{
		ABLOG(Error, TEXT("Failed to load staticmesh asset : %s"), *GateAssetPath);
	}

	// SM_SQAURE의 사방의 소켓에 문을 달아준다.
	static FName GateSockets[] = { {TEXT("+XGate") }, {TEXT("-XGate") }, {TEXT("+YGate") }, {TEXT("-YGate") }};
	for (FName GateSocket : GateSockets)
	{
		// 소켓이 있는지 확인
		ABCHECK(Mesh->DoesSocketExist(GateSocket));
		// 소켓에 문 애셋 부착해주기
		UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString());
		NewGate->SetStaticMesh(SM_GATE.Object);
		NewGate->SetupAttachment(RootComponent, GateSocket);
		// 문 애셋의 pivot이 왼쪽에 몰려있어서 Y축 -80.5f 만큼 조정해준다.
		NewGate->SetRelativeLocation(FVector(0.f, -80.5f, 0.f));
		GateMeshes.Add(NewGate);
	}
    ...

    이번엔 ABCharacter만을 감지하는 ABTrigger라는 이름의 콜리전 프리셋을 하나 추가한다.
    이 Collision Preset은 플레이어의 입장을 감지하고 섹션을 클리어한 후 출구를 선택할 때 사용한다.

    해당 프리셋을 사용하는 Box 컴포너트르 생성하고 섹션의 중아오가 각 철문 영역에 부착한다.
    ABSection.h
    ...
    // 철문의 Collision
	UPROPERTY(VisibleAnywhere, Category = Trigger, Meta = (AllowPrivateAccess = true))
	TArray<UBoxComponent*> GateTriggers;

	UPROPERTY(VisibleAnywhere, Category = Trigger, Meta = (AllowPrivateAccess = true))
	UBoxComponent* Trigger;
    ...
    ABSection.cpp
    ... 생성자
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Trigger->SetBoxExtent(FVector(775.f, 775.f, 300.f));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector(0.f, 0.f, 250.f));
	Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
    ...
    // SM_SQAURE의 사방의 소켓에 문을 달아준다.
	static FName GateSockets[] = { {TEXT("+XGate") }, {TEXT("-XGate") }, {TEXT("+YGate") }, {TEXT("-YGate") }};
	for (FName GateSocket : GateSockets)
	{
		// 소켓이 있는지 확인
		ABCHECK(Mesh->DoesSocketExist(GateSocket));
		// 소켓에 문 애셋 부착해주기
		UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString());
		NewGate->SetStaticMesh(SM_GATE.Object);
		NewGate->SetupAttachment(RootComponent, GateSocket);
		// 문 애셋의 pivot이 왼쪽에 몰려있어서 Y축 -80.5f 만큼 조정해준다.
		NewGate->SetRelativeLocation(FVector(0.f, -80.5f, 0.f));
		GateMeshes.Add(NewGate);

		// Collision 설정
		UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*GateSocket.ToString().Append(TEXT("Trigger")));
		NewGateTrigger->SetBoxExtent(FVector(100.f, 100.f, 300.f));
		NewGateTrigger->SetupAttachment(RootComponent, GateSocket);
		NewGateTrigger->SetRelativeLocation(FVector(70.f, 0.f, 250.f));
		NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		GateTriggers.Add(NewGateTrigger);
	}
    ...
    
*/