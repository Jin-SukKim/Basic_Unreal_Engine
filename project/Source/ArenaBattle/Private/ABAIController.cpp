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
	// 3초 설정
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
	// AI가 생성한 Blackboard 사용하도록 한다.
	if (UseBlackboard(BBAsset, BlackboardComponent))
	{
		// Behavior Tree 실행 전 Blackboard 데이터 세팅
		BlackboardComponent->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());

		// 생성한 Behavior Tree 애셋이 블랙보드 앳세과 함께 동작
		if (!RunBehaviorTree(BTAsset))
		{
			ABLOG(Error, TEXT("AIController couldn't run behavior tree!"));
		}
	}
}

void AABAIController::StopAI()
{
	// Behavior Tree Component 가져오기
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (nullptr != BehaviorTreeComponent)
	{
		// 로직 멈추기
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
	// 내비게이션 시스템이 이동 가능한 목적지를 랜덤으로 가져오는 함수
	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.f, NextLocation))
	{	
		// 폰을 목적지로 이동시키는 함수
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
		ABLOG(Warning, TEXT("Next Location : %s"), *NextLocation.Location.ToString());
	}
}
*/