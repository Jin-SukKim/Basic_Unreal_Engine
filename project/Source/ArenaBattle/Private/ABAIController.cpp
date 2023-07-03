// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AABAIController::HomePosKey(TEXT("HomePos"));
const FName AABAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AABAIController::TargetKey(TEXT("Target"));

AABAIController::AABAIController()
{
	/*
	// 3�� ����
	RepeatInterval = 3.f;
	*/

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Book/AI/BB_ABCharacter.BB_ABCharacter"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Book/AI/BT_ABCharacter.BT_ABCharacter"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	/*
	GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this,
		&AABAIController::OnRepeatTimer, RepeatInterval, true);
	*/
}

void AABAIController::RunAI()
{
	UBlackboardComponent* BlackboardComponent = Blackboard;
	// AI�� ������ Blackboard ����ϵ��� �Ѵ�.
	if (UseBlackboard(BBAsset, BlackboardComponent))
	{
		// Behavior Tree ���� �� Blackboard ������ ����
		BlackboardComponent->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());

		// ������ Behavior Tree �ּ��� ������ �ܼ��� �Բ� ����
		if (!RunBehaviorTree(BTAsset))
		{
			ABLOG(Error, TEXT("AIController couldn't run behavior tree!"));
		}
	}
}

void AABAIController::StopAI()
{
	// Behavior Tree Component ��������
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (nullptr != BehaviorTreeComponent)
	{
		// ���� ���߱�
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}

/*
void AABAIController::OnRepeatTimer()
{
	auto currentPawn = GetPawn();
	ABCHECK(nullptr != currentPawn);

	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (nullptr == NavSystem) return;

	FNavLocation NextLocation;
	// ������̼� �ý����� �̵� ������ �������� �������� �������� �Լ�
	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.f, NextLocation))
	{	
		// ���� �������� �̵���Ű�� �Լ�
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
		ABLOG(Warning, TEXT("Next Location : %s"), *NextLocation.Location.ToString());
	}
}
*/