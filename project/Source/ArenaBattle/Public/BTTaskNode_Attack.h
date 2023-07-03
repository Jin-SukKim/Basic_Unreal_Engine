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

	// 공격 애니메이션이 끝날 때까지 대기해야되 InProgress return 해 Task가 끝났다고 알린다.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// Tick 기능을 활성화해 FinishLatentTask 함수를 호출할 수 있게 해준다.
	// Tick에서 조건을 파악해 Task 종료 명령을 내리지 않으면 이 Task는 끝나지 않는다.
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsAttacking = false;
};
