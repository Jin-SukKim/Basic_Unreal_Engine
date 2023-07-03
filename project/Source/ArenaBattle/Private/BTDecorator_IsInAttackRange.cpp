// Fill out your copyright notice in the Description page of Project Settings.


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
	auto ControllingPawn = Cast<AABCharactter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
		return false;

	// 타겟 캐릭터 가져오기
	auto Target = Cast<AABCharactter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
	if (nullptr == Target)
		return false;

	// 목표와 AI의 거리가 충분히 가까운지 확인
	bResult = (Target->GetDistanceTo(ControllingPawn) <= ControllingPawn->GetFianlAttackRange());
	return bResult;
}
