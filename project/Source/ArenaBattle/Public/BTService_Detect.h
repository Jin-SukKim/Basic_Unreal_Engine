// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Detect.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTService_Detect : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_Detect();

protected:
	// Behavior Tree의 Service Node는 자신이 속한 Composite Node가 활성화될 경우 주기적으로 TickNode 함수를 호출한다.
	// 호출하는 주기는 Service Node 내부에 설정된 Interval 속성 값으로 지정할 수 있다
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
