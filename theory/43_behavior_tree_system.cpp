/*
    Behavior Tree System

    AI 컨트롤러에 특정 로직을 부여해 NPC의 행동을 제작할 수 있지만,
    좀 더 복잡한 NPC의 행동 패턴을 구현하려면 체계적인 모델에서 설계하는 것이 바람직하다.
    UE은 Behavior Tree Model과 이를 편집하는 에디터를 탑재하고 있어
    이를 사용해 AI 컨트롤러가 수행해야 하는 행동 패턴을 설계 할 수 있다.

    Behavior Tree는 NPC가 해야 할 행동(Behavior)을 분석하고 우선순위가 높은
    행동부터 NPC가 실행할 수 있도록 트리 구조로 설계하는 기법이다.

    Behavior Tree를 제작하기 위해 Behavior Tree와 Blackboard asset을 생성해야 한다.
    우클릭 -> Artificial Intelligence -> blackboard/Behavior Tree를 생성해준다.

    1. Blackboard : 블랙보드는 인공지능의 판단에 사용하는 데이터 집할을 의미한다.
        NPC의 의사 결정은 블랙보드에 있는 데이터를 기반으로 진행된다.

    2. Behavior Tree : 블랙보드 데이터에 기반해 설계한 Behavior Tree의 정보를 저장한 애셋이다.
        UE에서는 Behavior Tree를 시각화해 저장할 수 있는 편집 기능을 제공한다.

    Behavior Tree에서 우클릭해 Task 그룹의 Wait Task를 하나 생성하면
    폰에게 지정한 시간 동안 대기하라는 명령을 내린다.

    Task는 독립적으로 실핼될 수 없고 반드시 Composite 노드를 거쳐 실행돼야 한다.
    Composite 노드에는 대표적으로 Selector와 Sequence가 있다.

    연결된 Task들이 False 결과가 나올 때까지 왼쪽에서 오른쪽으로 Task를 계속 실행하는
    Sequence Composite을 추가해본다.
    Sequence Composite을 Root에 연결한 후 Wait task를 Composite에 연결한다.

    저장한 뒤 Blackboard와 Behavior Tree를 ABAIController가 사용하도록 코드를 추가한다.
    C++에서 Behavior Tree 관련 기능을 사용하기 위해선 AIModule 모듈을 추가해야 한다.
    ABAIController.h
    ...
	// Behavior Tree 사용
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	// Blackboard 사용
	UPROPERTY()
	class UBlackboardData* BBAsset;
    ...
    
    ABAIController.cpp
    ...
    #include "BehaviorTree/BehaviorTree.h"
    #include "BehaviorTree/BlackboardData.h"

    AABAIController::AABAIController()
    {

        static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Book/AI/BB_ABCharacter.BB_ABCharacter"));
        if (BBObject.Succeeded())
        {
            BBAsset = BBObject.Object;
        }

        static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Book/AI/BT_ABCharacter.BT_ABCharacter"));
        if (BTObject.Succeeded())
        {
            BTAsset = BTObject.Object;
        }
    }

    void AABAIController::OnPossess(APawn* InPawn)
    {
        Super::OnPossess(InPawn);

        UBlackboardComponent* BlackboardComponent = Blackboard;

        if (UseBlackboard(BBAsset, BlackboardComponent))
        {
            if (!RunBehaviorTree(BTAsset))
            {
                ABLOG(Error, TEXT("AIController couldn't run behavior tree!"));
            }
        }
    }

    Behavior Tree Asset을 열어두고 뷰 포트에서 ALt + P 로 게임을 실행하면 Behavior Tree 동작하는 것을 확인가능하다.

    NPC가 스테이지의 읨의의 위치로 순찰하는 기능을 구현해본다.

    Blackboard에는 특정 유형의 데이터를 저장하고 이를 Behavior Tree가 활용하도록 구성가능하다.

    순찰 기능을 위해 두 가지 데이터가 필요하다.

    1. NPC가 생성됐을 떄의 위치 값(Vector)
    2. 앞으로 NPC가 순찰항 위치 정보(Vector)

    이 두 값을 Blackboard에서 보관하도록 Vector타입으로 키를생성하고 이름을 부여해 저장한다.

    이제 Behavior Tree 구동전 AI Controller에서 Blackboard의 생성 위치 키값을 지정하도록 C++ 로직을 구현한다.
    Blackboard 키 이름 값으로 FName 속성을 추가하고 키 이름과 같은 값을 할당한다.
    ABAIController.h
    ...
    // 이름이 변경되지 않는다는 가정하에 static const로 설정
	static const FName HomePosKey;
	static const FName PatrolPosKey;
    ...
    ABAIController.cpp
    ...
    #include "BehaviorTree/BlackboardComponent.h"
    const FName AABAIController::HomePosKey(TEXT("HomePos"));
    const FName AABAIController::PatrolPosKey(TEXT("PatrolPos"));
    ...
    void AABAIController::OnPossess(APawn* InPawn)
    {
        Super::OnPossess(InPawn);

        UBlackboardComponent* BlackboardComponent = Blackboard;
        // AI가 생성한 Blackboard 사용하도록 한다.
        if (UseBlackboard(BBAsset, BlackboardComponent))
        {	
            // Behavior Tree 실행 전 Blackboard 데이터 세팅
            BlackboardComponent->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());

            // 생성한 Behavior Tree 애셋이 블랙보드 앳세과 함께 동작
            if (!RunBehaviorTree(BTAsset))
            {
                ABLOG(Error, TEXT("AIController couldn't run behavior tree!"));
            }
        }
    }
    ...

    Blackboard 값이 제대로 세팅 되면 NPC가 이동할 위치인 PatrolPos 데이터를 생성해야한다.
    이는 순찰할 때마다 바뀌므로 Task를 제작해 Behavior Tree에서 Blackboard에 값을 쓰도록 설계하는 것이 좋다.

    BTTaskNode를 부모 클래스로 하는 C++ 클래스를 생성한다.
    해당 Task의 이름은 BTTask_FindPatrolPos로 지정한다.
    UI에서 표현할 때는 BTTask_ 접두사 부분이 자동으로 걸러진다.

    Behavior Tree 에디터에서 사용할 Task를 추가하기 위해선 GameplayTasks 모듈을 추가해줘야 한다.ㄴ

    Behacvior Tree는 Task를 실행할 때 Task 클래스의 ExecuteTask라는 멤버함수를 싱행한다.
    ExecuteTask 함수는 셋 중 하나의 값을 반환해야 한다.

    EBTNodeResult::Type인 namespace의 enum에 선언되있다.
    1. Aborted : Task 실행 중에 중단됐다. 결과적으로 실패했다.
    2. Failed : Task를 수행했지만 실패했다.
    3. Succeeded : Task를 성공적으로 수행했다.
    4. InProgress : Task를 계속 수행하고 있다. Task의 실행 경과는 향후 알려줄 예정이다.

    ExecuteTask 함수의 실행 결과에 따라 Composite 내에 있는 다음 Task를 계속 수행할지 중단할지 결정된다.
    현재 사용중인 Sequence Composite은 자신에 속한 Task를 실패할때까지 계속 실행한다.

    ExecuteTask 함수에서 다음 정찰 지점을 찾는 로직을 구현하고 실행 결과를 바로 반환하도록 한다.
    추가로 Task의 이름을 다른 이름으로 표시하고 싶다면 NodeName 속성을 다른 값으로 지정하면된다.
    BTTask_FindPatrolPos.h
    ...
    public:
	UBTTask_FindPatrolPos();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	...
    BTTask_FindPatrolPos.cpp
    ...
    #include "BehaviorTree/BlackboardComponent.h"
    #include "NavigationSystem.h"

    UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
    {
        // Task의 이름을 다른 이름으로 표시하고 싶다면 NodeName 속성을 다른 값으로 지정
        NodeName = TEXT("FindPatrolPos");
    }

    EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
    {
        EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

        // 가져올 Pawn이 없다면 AI가 필요없으므로 Task 실패
        auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
        if (nullptr == ControllingPawn)
            return EBTNodeResult::Failed;

        // Navigation System이 없다면 목적지로 이동을 못시키므로 실패
        UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
        if (nullptr == NavSystem)
            return EBTNodeResult::Failed;

        FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AABAIController::HomePosKey);
        FNavLocation NextPatrol;
        // 내비게이션 시스템이 이동 가능한 목적지를 랜덤으로 가져오는 함수
        if (NavSystem->GetRandomPointInNavigableRadius(Origin, 500.f, NextPatrol))
        {
            // Blackboard의 키 값인 PatrolPosKey의 값 세팅
            OwnerComp.GetBlackboardComponent()->SetValueAsVector(AABAIController::PatrolPosKey, NextPatrol.Location);
            return EBTNodeResult::Succeeded;
        }

        return EBTNodeResult::Failed;
    }

    새로운 Task 제작을 완료하면 기존의 Wait Task 오른쪽이 배치하고 그 오른쪽엔 UE가 제공하는
    MoveTo Task를 추가해 배치한다. (모두 Sequence Composite에 연결하면 왼쪽부터 순차적으로 실행된다.)
    이떄 MoveTo Task의 Blackboard Key 값을 PatrolPos로 변경해줘야한다.
    Sequence Composite에 의해 Wait Task가 성공하면 FindPatrolPos Task를 수행하고
    성공하면 FindPAtrolPos에서 설정한 Blackboard의 PatrolPos 키 값을 참고해 MoveTo Task가 실행된다.

*/