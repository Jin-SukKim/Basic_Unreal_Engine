// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Attack.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTTaskNode_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_Attack();

	// ���� �ִϸ��̼��� ���� ������ ����ؾߵ� InProgress return �� Task�� �����ٰ� �˸���.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// Tick ����� Ȱ��ȭ�� FinishLatentTask �Լ��� ȣ���� �� �ְ� ���ش�.
	// Tick���� ������ �ľ��� Task ���� ����� ������ ������ �� Task�� ������ �ʴ´�.
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsAttacking = false;
};
