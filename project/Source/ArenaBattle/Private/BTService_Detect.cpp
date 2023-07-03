// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "ABAIController.h"
#include "ABCharactter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	// TickNode �Լ��� ȣ��Ǵ� �ֱ�
	Interval = 1.f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AI�� ��Ʈ���ϴ� ĳ����
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	// ĳ������ ��ġ
	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	// ĳ���� �ݰ� 6m
	float DetectRadius = 600.f;

	// ĳ���Ͱ� ���忡 �ִ��� Ȯ��
	if (nullptr == World) return;
	// �ݰ� �� ������ ��� ĳ���� ������ ����� �����ϴµ� ������ UE�� �ڷᱸ���� TArray�� ���޵ȴ�.
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionqueryParam(NAME_None, false, ControllingPawn);
	// �ݰ泻�� ��� ĳ���͸� �����ϱ����� OverlapMultiByChannel ���
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
			// ĳ���Ͱ� �����ϰ� PlayerController�� ��Ʈ���ϴ��� Ȯ��
			if (ABCharacter && ABCharacter->GetController()->IsPlayerController())
			{
				// blackboard�� Target�� ���� PlayerController�� ��Ʈ���ϴ� ĳ���ͷ� ����
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
