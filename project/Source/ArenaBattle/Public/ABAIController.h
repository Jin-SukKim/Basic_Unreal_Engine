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

	// �̸��� ������� �ʴ´ٴ� �����Ͽ� static const�� ����
	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;

	// state�� �°� behavior tree ������ �������� ����/����
	void RunAI();
	void StopAI();
private:
	/*
	// C++���� AI �����ϴ� ����
	// 3�ʸ��� ������ �������� �̵��ϴ� ����� ������ ���� Ÿ�̸�
	void OnRepeatTimer();

	// Ÿ�̸�
	FTimerHandle RepeatTimerHandle;
	float RepeatInterval;
	*/

	// Behavior Tree ���
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	// Blackboard ���
	UPROPERTY()
	class UBlackboardData* BBAsset;
};
