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

	// AI�� ��Ʈ���ϴ� ĳ���� ��������
	auto ControllingPawn = Cast<AABCharactter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
		return false;

	// Ÿ�� ĳ���� ��������
	auto Target = Cast<AABCharactter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
	if (nullptr == Target)
		return false;

	// ��ǥ�� AI�� �Ÿ��� ����� ������� Ȯ��
	bResult = (Target->GetDistanceTo(ControllingPawn) <= ControllingPawn->GetFianlAttackRange());
	return bResult;
}
