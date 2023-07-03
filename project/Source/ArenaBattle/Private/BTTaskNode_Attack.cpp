// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Attack.h"
#include "ABAIController.h"
#include "ABCharactter.h"

UBTTaskNode_Attack::UBTTaskNode_Attack()
{
	// Tick ��� Ȱ��ȭ
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

	//	���� �ִϸ��̼��� ���� ������ ����ؾ� �ϴ� ���� Task�̹Ƿ� InProgress return
	return EBTNodeResult::InProgress;
}

void UBTTaskNode_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsAttacking)
	{
		// Task�� Inprogress
		// ������ ���� Task�� �����ٰ� �˷��� Behavior Tree System�� �� Task���� �������� �Ѿ�� �Ѵ�.
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
