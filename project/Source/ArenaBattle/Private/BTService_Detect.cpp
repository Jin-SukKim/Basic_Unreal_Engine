// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "ABAIController.h"
#include "ABCharactter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	// TickNode 함수가 호출되는 주기
	Interval = 1.f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AI가 컨트롤하는 캐릭터
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	// 캐릭터의 위치
	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	// 캐릭터 반경 6m
	float DetectRadius = 600.f;

	// 캐릭터가 월드에 있는지 확인
	if (nullptr == World) return;
	// 반경 내 감지된 모든 캐릭터 정보는 목록을 관리하는데 적합한 UE의 자료구조인 TArray로 전달된다.
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionqueryParam(NAME_None, false, ControllingPawn);
	// 반경내의 모든 캐릭터를 감지하기위해 OverlapMultiByChannel 사용
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionqueryParam
	);

	if (bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			AABCharactter* ABCharacter = Cast<AABCharactter>(OverlapResult.GetActor());
			// 캐릭터가 존재하고 PlayerController가 컨트롤하는지 확인
			if (ABCharacter && ABCharacter->GetController()->IsPlayerController())
			{
				// blackboard의 Target의 값을 PlayerController가 컨트롤하는 캐릭터로 설정
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, ABCharacter);
				
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
				DrawDebugPoint(World, ABCharacter->GetActorLocation(), 10.f, FColor::Blue, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), ABCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);
	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}
