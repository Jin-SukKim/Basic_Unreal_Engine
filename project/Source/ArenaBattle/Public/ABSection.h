// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABSection.generated.h"

UCLASS()
class ARENABATTLE_API AABSection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABSection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 에디터와 연동되는 함수로 에디터 작업을 하면 플레이하지 않아도 변경사항을 볼 수 있다.
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	// state machine
	enum class ESectionState : uint8
	{
		READY = 0,
		BATTLE,
		COMPLETE
	};

	// state machine setting
	void SetState(ESectionState NewState);
	ESectionState CurrentState = ESectionState::READY;

	// 다음 섹션으로 넘어갈지 게이트 콜리전 설정
	void OperateGates(bool bOpen = true);

	// Gate를 넘어가면 섹션을 생성하거나 이미 생성된 섹션이 있는지 확인하기 위해 Delegate 설정
	// 섹션 Delegate에 연동할 함수
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 게이트 delegate에 연동할 함수
	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 전투 없이 다음 섹션으로 넘어갈 수 있게 
	UPROPERTY(EditAnywhere, Category = State, Meta = (AllowPrivateAccess = true))
	bool bNoBattle;

	UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = ture))
	UStaticMeshComponent* Mesh;

	// 소켓에 부착될 철문들
	UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = true))
	TArray<UStaticMeshComponent*> GateMeshes;

	// 철문의 Collision
	UPROPERTY(VisibleAnywhere, Category = Trigger, Meta = (AllowPrivateAccess = true))
	TArray<UBoxComponent*> GateTriggers;

	UPROPERTY(VisibleAnywhere, Category = Trigger, Meta = (AllowPrivateAccess = true))
	UBoxComponent* Trigger;

	// NPC 생성
	void OnNPCSpawn();

	// 생성될 시간
	UPROPERTY(EditAnywhere, Category = Spawn, Meta = (AllowPrivateAccess = true))
	float EnemySpawnTime;

	UPROPERTY(EditAnywhere, Category = Spawn, Meta = (AllowPrivateAccess = true))
	float ItemBoxSpawnTime;
	
	FTimerHandle SpawnNPCTimerHandle = {};
	FTimerHandle SpawnItemBoxTimerHandle = {};

	// 액터가 제거될 때 호출되는 함수
	UFUNCTION()
	void OnKeyNPCDestroyed(AActor* DestroyedActor);
};
