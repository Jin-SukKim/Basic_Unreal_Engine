/*
    AIConstroller와 내비게이션 시스템

    UE의 Behavior Tree 모델을 사용해 AI를 설계하고 플레이어가 아닌 AI에 의해
    정찰하고 플레이어를 쫒아와 공격하는 NPC를 제작한다.

    플레이어가 컨트롤하지 않고 레벨에 배치돼 스스로 행동하는 캐릭터를 NPC(Non Player Character)라고 한다.
    이 NPC에 AI를 추가해 행동하도록 만들 수 있다.

    UE에는 컴퓨터가 AI로 NPC를 제어하도록 AI Controller를 제공한다.
    폰은 플레이어 컨트롤러와 동일한 방식으로 AI 컨트롤러에 빙의될 수 있으며, 이때부턴 AI가 컨트롤한다.

    AIController를 부모 클래스로 하는 클래스를 생성한다.
    그리고 ABCharacter 클래스가 이를 사용하도록AIController의 클래스 속성을
    ABAIController 클래스로 정하고 AI의 생성 옵션을 PlaceInWorldOrSpawned로 설정한다.

    그러면 앞으로 레벨에 배치하거나 새롭게 생성되는 ABCharacter마다 ABAIController 액터가 생성되고,
    플레이어가 조종하는 캐릭터를 제외한 모든 캐릭터는 이 AI Controller의 지배를 받는다.

    ABCharacter.cpp
    #include "ABAIController.h"
    ...생성자
    // ABCharacter의 AIController를 생성한 ABAIController로 설정
	AIControllerClass = AABAIController::StaticClass();
	// 플레이어가 조종하는 캐릭터를 제외한 레벨에 배치되거나 생성되는 모든 캐릭터가 ABAIController의 지배를 받는다.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    ...

    이 후 캐릭터를 보면 AI Controller Class가 생성한 컨트롤러로 변겨오디어 있고 Auto Possess 옵션도 설정되있다.

    내비게이션 시스템

    NPC는 스스로 움직여야 하기에 이를 보조할 장치가 필요한데, 이때 많이 사용하는 것이 
    내비게이션 메시(Navigation Mesh) 기반의 길 찾기 시스템이다.
    로직을 구현하기 전 먼저 레벨에 내비게이션 베시를 배치가 환경을 구축시켜야 한다.

    왼쪽 상단의 Get Content -> Volume -> NavMeshBoudnsVolume 배치

    크기는 넉넉하게 10000x10000x500으로 잡았다.

    언리얼 에디터에서 뷰포트를 선택하고 P키를 입력하면 에디터에서 빌드한 내비 메시 영역이 녹색으로표시된다.

    NavMesh 영역을 생성하면, 이를 활요해 ABAIController에 빙의한 폰에게 목적지를 알려줘 
    폰이 목적지까지 스스로 움직이는 명령을 추가한다.
    그리고 AI Controller에 타이머를 설치해 3초마다 폰에게 목적지로 이동하는 명령을 내린다.

    UE의 Navigation System은 이동 가능한 목적지를 랜덤으로 가져오는 GetRandomPoinInNavigableRadius 함수와
    목적지로 폰을 이동시키는 SimpleMoveToLocation 함수를 제공한다.

    Navigation을 사용하기위해 NavigationSystem 모듈을 추가해줘야 한다.

    ABAIController.h
    public:
        AABAIController();
        virtual void OnPossess(APawn* InPawn) override;

    private:
        // 3초마다 폰에게 목적지로 이동하는 명령을 내리기 위한 타이머
        void OnRepeatTimer();

        // 타이머
        FTimerHandle RepeatTimerHandle;
        float RepeatInterval;
    ...
    ABAIController.cpp
    #include "NavigationSystem.h"
    #include "Blueprint/AIBlueprintHelperLibrary.h"

    AABAIController::AABAIController()
    {
        // 3초 설정
        RepeatInterval = 3.f;
    }

    void AABAIController::OnPossess(APawn* InPawn)
    {
        Super::OnPossess(InPawn);
        GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this,
            &AABAIController::OnRepeatTimer, RepeatInterval, true);
    }

    void AABAIController::OnRepeatTimer()
    {
        auto currentPawn = GetPawn();
        ABCHECK(nullptr != currentPawn);

        UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
        if (nullptr == NavSystem) return;

        FNavLocation NextLocation;
        // 내비게이션 시스템이 이동 가능한 목적지를 랜덤으로 가져오는 함수
        if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.f, NextLocation))
        {	
            // 폰을 목적지로 이동시키는 함수
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
            ABLOG(Warning, TEXT("Next Location : %s"), *NextLocation.Location.ToString());
        }
    }


*/