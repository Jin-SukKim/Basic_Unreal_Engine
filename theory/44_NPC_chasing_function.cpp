/*
    NPC의 추격 기능 구현

    NPC가 정찰 중 플레이어 발견시 추격하도록 기능을 추가한다.

    NPC가 플레이어 발견 시 플레이어의 정보를 Blackboard에 저장하도록 Object 타입으로 Target 변수를 생성한다.
    Blackboard의 Object 타입에서는 기반 클래스(Base Class)를 지정할 수 있는데, 기반 클래스는 ABCharacter로 지정한다.

    이제 AI는 추격과 정찰 중 하나를 선택해 행동하기에 Selector Composite을 사용해 로직을 확장해준다.
    둘 중 추격에 더 우선권을 주고, 추격 로직은 Blackboard의 Target을 향해 이동하도록
    Behavior Tree 설계를 확장한다.

    Root -> Selector -> Sequence -> 추격 
                     -> Sequence -> 탐색

    정찰 중 플레이어가 일정 반경 내에 있으면 감지해 추격하는 기능을 넣어준다.
    UE는 이를 위해 Service Node를 제공한다.

    Sevice Node는 독립적으로 동작하지 않고 Composite Node에 부착되는 노드다.
    또한 Service Node는 해당 Composite에 속한 Task들이 실행되는 동안 반복적인
    작업을 싱행하는데 적합하다.

    플레이어를 감지하는 Service 노드를 새로 생성하고 Selector Composite에 추가하면 
    플레이어를 감지하는 루틴을 반복한다.

    새로운 Service 제작을 위해 BTService를 부모로 하는 BTService_Detect 클래스를 생성한다.

    Behavior Tree의 Service Node는 자신이 속한 Composite Node가 활성화될 경우 주기적으로
    TickNode 함수를 호출한다.
    호출하는 주기는 Service Node 내부에 설정된 Interval 속성 값으로 지정할 수 있따.

    TickNode 함수에 NPC의 위치를 기준으로 반경 6m 내에 캐릭터가 있는지 감지하고
    다른 NPC ABCharacter가 있는 경우도 가정해 모든 캐릭터를 감지하는 OverlapMultyByChannel 함수를 사용한다.
    반경 내 감지된 모든 캐릭터 정보는 목록을 관리하는데 적합한 UE의 자료구조인 TArray로 전달된다.
    BTService_Detect.h
    ...
    public:
        UBTService_Detect();

    protected:
        // Behavior Tree의 Service Node는 자신이 속한 Composite Node가 활성화될 경우 주기적으로 TickNode 함수를 호출한다.
        // 호출하는 주기는 Service Node 내부에 설정된 Interval 속성 값으로 지정할 수 있다
        virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    ...
    BTService_Detect.cpp
    #include "BTService_Detect.h"
    #include "ABAIController.h"
    #include "ABCharactter.h"
    #include "BehaviorTree/BlackboardComponent.h"
    #include "DrawDebugHelpers.h"

    UBTService_Detect::UBTService_Detect()
    {
        NodeName = TEXT("Detect");
        // TickNode 함수가 호출되는 주기
        Interval = 1.f;
    }

    void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
    {
        Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

        // AI가 컨트롤하는 캐릭터
        APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
        if (nullptr == ControllingPawn) return;

        // 캐릭터의 위치
        UWorld* World = ControllingPawn->GetWorld();
        FVector Center = ControllingPawn->GetActorLocation();
        // 캐릭터 반경 6m
        float DetectRadius = 600.f;

        // 캐릭터가 월드에 있는지 확인
        if (nullptr == World) return;
        // 반경 내 감지된 모든 캐릭터 정보는 목록을 관리하는데 적합한 UE의 자료구조인 TArray로 전달된다.
        TArray<FOverlapResult> OverlapResults;
        FCollisionQueryParams CollisionqueryParam(NAME_None, false, ControllingPawn);
        // 반경내의 모든 캐릭터를 감지하기위해 OverlapMultiByChannel 사용
        bool bResult = World->OverlapMultiByChannel(
            OverlapResults,
            Center,
            FQuat::Identity,
            ECollisionChannel::ECC_GameTraceChannel1,
            FCollisionShape::MakeSphere(DetectRadius),
            CollisionqueryParam
        );

        DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
    }

    서비스가 만들어지면 Selector Composite을 우클릭 -> Add Service -> Detect를 선택해 부착한다.
    이러면 NPC는 매초 반경 6미터를 탐지한다.

    NPC가 탐지 영역 내의 캐릭터를 감지한다면, 그중에서 조종하는 캐릭터를 추려야한다.
    캐릭터를 조종하는 컨트롤러가 Player Controller인지 파악할 수 있도록 IsPlayerController 함수를 사용한다.
    탐지되면 Blackboad의 Target 값을 플레이어 캐릭터로 지정하고, 그렇지 않으면 nullptr 값을 지정한다.
    또한 플레이어 캐릭터를 감지하면 녹색으로 구체를 그리고, NPC와 캐릭터까지 연결된 선을 추가로 그려준다.
    ABAIController.h
    ...
    static const FName TargetKey;
    ...
    ABAIController.cpp
    ...
    const FName AABAIController::TargetKey(TEXT("Target"));
    ...
    BTService_Detect.cpp
    ...
    void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
    {
        ...
        if (bResult)
        {
            for (auto const& OverlapResult : OverlapResults)
            {
                AABCharactter* ABCharacter = Cast<AABCharactter>(OverlapResult.GetActor());
                // 캐릭터가 존재하고 PlayerController가 컨트롤하는지 확인
                if (ABCharacter && ABCharacter->GetController()->IsPlayerController())
                {
                    // blackboard의 Target의 값을 PlayerController가 컨트롤하는 캐릭터로 설정
                    OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, ABCharacter);
                    
                    DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
                    DrawDebugPoint(World, ABCharacter->GetActorLocation(), 10.f, FColor::Blue, false, 0.2f);
                    DrawDebugLine(World, ControllingPawn->GetActorLocation(), ABCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);
                    return;
                }
            }
        }
	    DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
    }

    NPC는 플레이어를 감지하면 플레이어를 계속 따라다닌다.
    하지만 NPC가 이동할 때 회전이 부자연스러운데 이를 보강하기위해 별도로
    NPC를 위한 ControlMode를 추가하고 NPC는 이동 방향에 따라 회전하도록
    캐릭터 무브먼트 설정을 변경한다.
    추가로 NPC의 최대 이동 속도를 플레이어보다 낮게 설정해 NPC로부터 도망갈 수 있도록 만들어준다.

    ABChacter.h
    ...
    // class로 선언해 묵시적 변환을 허용하지 않는다.
	enum class EControlMode
	{
		TPS,
		QUARTERVIEW,
		NPC
	};
    ...
    // AI가 컨트롤하는 NPC는 NPC 컨트롤 모드 사용
	virtual void PossessedBy(AController* NewController) override;
    ...
    ABCharacter.cpp
    ...
    void AABCharactter::SetControlMode(EControlMode ControlMode)
    {
    ...
    case EControlMode::NPC:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
		break;
    ...
    }
    ...
    void AABCharactter::PossessedBy(AController* NewController)
    {
        Super::PossessedBy(NewController);

        if (IsPlayerControlled())
        {
            SetControlMode(EControlMode::TPS);
            GetCharacterMovement()->MaxWalkSpeed = 600.f;
        }
        else
        {
            SetControlMode(EControlMode::NPC);
            GetCharacterMovement()->MaxWalkSpeed = 300.f;
        }
    }

    이제 서비스가 실행된 결과에 따라 Selector Decorator 왼쪽의 추격과 오른쪽 정찰 로직이 나눠지도록
    Behavior Tree 로직을 구성한다.
    Service 결과는 Blackboard의 Target 키에 값이 있는지 없는지로 구분할 수 있다.

    이를 위해 Blackboard의 값을 기반으로 특정 Composite의 실행 여부를 결정하는 Decorator 노드를
    사용하는 것을 추천한다.
    왼쪽 sequence composite에 blackboard decolator 노드를 삽입한다.

    Sequence node 우클릭 -> Add Decolator -> Blackboard -> detail 창 
    -> blackboard key 값을 Target으로 변경, Notify Observer 값은 On Value Change로 변경, Observer aborts를 self로 변경해준다.
    Observer aborts(관찰자 중단) 값을 설정하지 않으면 Composite에 속한 Task가 마무리될 때까지 대기하므로 플레이어가 시야를 벗어나도
    NPC는 플레이어를 따라잡을 때까지 계속 쫓아온다.

    오른쪽 Sequnce도 동일하게 Decolator를 추가하나 반대 조건인 Is Not Set으로 설정해준다.
    Observer absorts도 설정해주면 정찰 중에 플레이어 감지시 정찰을 중단하고 바로 추격을 시작한다.
*/