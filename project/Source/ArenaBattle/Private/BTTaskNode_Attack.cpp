// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Attack.h"
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
