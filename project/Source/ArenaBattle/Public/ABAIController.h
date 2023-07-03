// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "AIController.h"
#include "ABAIController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AABAIController();
	virtual void OnPossess(APawn* InPawn) override;

	// 이름이 변경되지 않는다는 가정하에 static const로 설정
	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;

	// state에 맞게 behavior tree 로직을 수동으로 구동/중지
	void RunAI();
	void StopAI();
private:
	/*
	// C++에서 AI 설정하는 로직
	// 3초마다 폰에게 목적지로 이동하는 명령을 내리기 위한 타이머
	void OnRepeatTimer();

	// 타이머
	FTimerHandle RepeatTimerHandle;
	float RepeatInterval;
	*/

	// Behavior Tree 사용
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	// Blackboard 사용
	UPROPERTY()
	class UBlackboardData* BBAsset;
};
