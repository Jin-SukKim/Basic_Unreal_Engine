/*
    NPC의 공격

    추격 로직을 발전시켜 공격하는 기능을 추가한다.
    NPC의 행동은 플레이어와의 거리에 따라 행동이 달라지기에 Selector Node로
    추격과 공격으로 확장해준다.

    이번에는 Blackboard의 값을 참조하지 않고 목표물인 플레이어가 공격 범위 내에있는지
    판단하는 Decolator를 하나 생성한다.

    BTDecorator 클래스를 부모로 하는 클래스 BTDecorator_IsInAttackRange를 생성해준다.
    
    Decorator 클래스는 CalculateRawConditionValue 함수를 상속받아 원하는 조건이 달성됐는지
    파악하도록 설계됐다.
    이 함수는 const로 선언돼 데코레이터 클래스의 멤버 변수 값은 변경할 수 없다.
    BTDecorator_IsInAttackRange.h
    ...
    public:
        UBTDecorator_IsInAttackRange();

    protected:
        // 원하는 조건이 달성됐는지 파악하는 함수
        virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
    ..
    BTDecorator_IsInAttackRange.cpp
    #include "BTDecorator_IsInAttackRange.h"
    #include "ABAIController.h"
    #include "ABCharactter.h"
    #include "BehaviorTree/BlackboardComponent.h"

    UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
    {
        NodeName = TEXT("CanAttack");
    }

    bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
    {
        bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

        // AI가 컨트롤하는 캐릭터 가져오기
        auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
        if (nullptr == ControllingPawn)
            return false;

        // 타겟 캐릭터 가져오기
        auto Target = Cast<AABCharactter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
        if (nullptr == Target)
            return false;

        // 목표와 AI의 거리가 충분히 가까운지 확인
        bResult = (Target->GetDistanceTo(ControllingPawn) <= 200.f);
        return bResult;
    }

    완성된 Decorator를 가장 왼쪽의 공격 Sequence Composite에 부착한다.
    Decorator의 조건이 True면 공격을 수행하는데 공격 기능을 구현해줘야 한다.

    공격이 1.5초간 수행됐다고 가정한 후 Wait Task를 왼쪽 Sequence Composite에 부착한다.
    이러면 NPC가 플레이어를 따라잡는 경우 NPC는 1.5초간 대기하고 다시 추격한다.

    반대쪽 오른쪽 Sequence Composite에 동일한 Decorator를 추가하고 Inverse Condition 속성 값을 체크해
    조건을 반대로 설정한다.

    Wait 대신 공격 Task를 생성한다.

    BTTaskNode를 부모로 하는 BTTask_Attack 클래스를 생성한다.
    공격 Task는 공격 애니메이션이 끝날 떄까지 대기해야 하는 지연 Task이므로 ExecuteTask의 결과 값을
    InProgress로 일단 Return하고 공격이 끝났을 떄 Task가 끝났다고 알려줘야 한다.

    FinishLatentTask가 공격이 끝남을 알려준다.
    Task에서 이 함수를 나중에 호출해주지 않으면 Behavior tree system은 현재 Task에 계속 머물게 된다.

    차후 FinishLatentTask를 호출할 수 있도록 노드의 Tick 기능을 활성화하고 Tick에서 조건을 파악한 뒤
    Task 종료 명령을 내려준다.
    BTTask_Attack.h
    ...
    public:
        UBTTaskNode_Attack();

        // 공격 애니메이션이 끝날 때까지 대기해야되 InProgress return 해 Task가 끝났다고 알린다.
        virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    protected:
        // Tick 기능을 활성화해 FinishLatentTask 함수를 호출할 수 있게 해준다.
        // Tick에서 조건을 파악해 Task 종료 명령을 내리지 않으면 이 Task는 끝나지 않는다.
        virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    ...
    BTTask_Attack.cpp
    UBTTaskNode_Attack::UBTTaskNode_Attack()
    {
        // Tick 기능 활성화
        bNotifyTick = true;
    }

    EBTNodeResult::Type UBTTaskNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
    {
        EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
        //	공격 애니메이션이 끝날 떄까지 대기해야 하는 지연 Task이므로 InProgress return
        return EBTNodeResult::InProgress;
    }

    void UBTTaskNode_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
    {
        Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
        // Task가 Inprogress
        // 공격이 끝나 Task가 끝났다고 알려줘 Behavior Tree System이 이 Task에서 다음으로 넘어가게 한다.
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }

    캐릭터에 공격 명령을 내리고 공격이 끝난 시점을 파악해 태스크를 종료하도록 한다.
    먼저 AI Controller에서도 공격 명령을 내릴 수 있도록 ABCharacter 클래스의 Attack 함수의 접근 권한을 public으로 하고
    플레이어의 공격이 종료되면 공격 Task에서 해당 알림을 받을 수 있도록 Delegate를 선언하고 공격 종료시 호출되게 한다.
    캐릭터의 Delegate 설정이 완료되면 Task에서 Lambda 함수를 해당 Delegate에 등록하고 Tick 함수 로직에서
    이르 파악해 FinishlAtentTask 함수를 호출해 Task를 종료하도록 한다.
    ABCharacter.h
    ...
    DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);
    ...
    public:
        ...
        // 공격 함수
        void Attack();
        // Attack Task에서 사용할 수 있도록 Delegate 선언
        FOnAttackEndDelegate OnAttackEnd;
    ...
    ABCharacter.cpp
    ...
    void AABCharactter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
    {
        ...
        // 공격 애니메이션이 끝나면 Task가 끝나야 하므로 Delegate로 알려준다.
        OnAttackEnd.Broadcast();
    }
    BTTask_Attack.h
    ...
    bool IsAttacking = false;
    ...
    BTTask_Attack.cpp
    ...
    #include "ABAIController.h"
    #include "ABCharactter.h"

    UBTTaskNode_Attack::UBTTaskNode_Attack()
    {
        // Tick 기능 활성화
        bNotifyTick = true;
        IsAttacking = false;
    }

    EBTNodeResult::Type UBTTaskNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
    {
        EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
        
        auto ABCharacter = Cast<AABCharactter>(OwnerComp.GetAIOwner()->GetPawn());
        if (nullptr == ABCharacter)
            return EBTNodeResult::Failed;

        ABCharacter->Attack();
        IsAttacking = true;
        ABCharacter->OnAttackEnd.AddLambda([this]() -> void {
            IsAttacking = false;
            });

        //	공격 애니메이션이 끝날 떄까지 대기해야 하는 지연 Task이므로 InProgress return
        return EBTNodeResult::InProgress;
    }

    void UBTTaskNode_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
    {
        Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
        if (!IsAttacking)
        {
            // Task가 Inprogress
            // 공격이 끝나 Task가 끝났다고 알려줘 Behavior Tree System이 이 Task에서 다음으로 넘어가게 한다.
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }

    이제 Wait을 Attack Task로 교쳬한다.
    한가지 문제가 있는데 NPC가 플레이어를 공격할 때 제자리에 정지하기에
    플레이어가 뒤로 돌아가도 계속 같은 곳을 공격한다

    이를 보완하기 위해 공격하면서 동시에 플레이어를 향해 회전하는 기능을 추가한다.

    Blackboar의 Target으로 회전하는 Task를 추가한다.
    BTTaskNode를 부모 클래스로 하는 BTTask_TurnToTarget 클래스를 생성한다.
    이 Task는 플레이어 폰을 향해 일정한 속도로 회전하도록 FMath::RInterpTo 함수를 사용해
    회전시키는 기능을 구현한다.
    BTTask_TurnToTarget.h
    ...
    public:
	UBTTask_TurnToTarget();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    ...
    BTTask_TurnToTarget.cpp
    #include "BTTask_TurnToTarget.h"
    #include "ABAIController.h"
    #include "ABCharactter.h"
    #include "BehaviorTree/BlackboardComponent.h"

    UBTTask_TurnToTarget::UBTTask_TurnToTarget()
    {
        NodeName = TEXT("Turn");
    }

    EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
    {
        EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

        auto ABCharacter = Cast<AABCharactter>(OwnerComp.GetAIOwner()->GetPawn());
        if (nullptr == ABCharacter)
            return EBTNodeResult::Failed;

        auto Target = Cast<AABCharactter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
        if (nullptr == Target)
            return EBTNodeResult::Failed;

        
        // Target을 바라보는 벡터 계산
        FVector LookVector = Target->GetActorLocation() - ABCharacter->GetActorLocation();
        LookVector.Z = 0.f;
        // 회전 벡터 계산
        FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
        // 회전
        ABCharacter->SetActorRotation(FMath::RInterpTo(ABCharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.f));

        return EBTNodeResult::Succeeded;
    }

    회전 Task를 완성하면 공격 로직에서 사용한 Sequence Composite을 Simple Parallel Composite으로 
    대체한 후 캐릭터의 공격을 메인 Task로, 회전을 보조 Task로 지정한다.

    Simple Parallel Composite에 의해 캐릭터는 공격과 캐릭터를 향해 회전하는 Task를 동시에 실행한다.
*/