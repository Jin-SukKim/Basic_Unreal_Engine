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

	// �����Ϳ� �����Ǵ� �Լ��� ������ �۾��� �ϸ� �÷������� �ʾƵ� ��������� �� �� �ִ�.
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

	// ���� �������� �Ѿ�� ����Ʈ �ݸ��� ����
	void OperateGates(bool bOpen = true);

	// Gate�� �Ѿ�� ������ �����ϰų� �̹� ������ ������ �ִ��� Ȯ���ϱ� ���� Delegate ����
	// ���� Delegate�� ������ �Լ�
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ����Ʈ delegate�� ������ �Լ�
	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ���� ���� ���� �������� �Ѿ �� �ְ� 
	UPROPERTY(EditAnywhere, Category = State, Meta = (AllowPrivateAccess = true))
	bool bNoBattle;

	UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = ture))
	UStaticMeshComponent* Mesh;

	// ���Ͽ� ������ ö����
	UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = true))
	TArray<UStaticMeshComponent*> GateMeshes;

	// ö���� Collision
	UPROPERTY(VisibleAnywhere, Category = Trigger, Meta = (AllowPrivateAccess = true))
	TArray<UBoxComponent*> GateTriggers;

	UPROPERTY(VisibleAnywhere, Category = Trigger, Meta = (AllowPrivateAccess = true))
	UBoxComponent* Trigger;

	// NPC ����
	void OnNPCSpawn();

	// ������ �ð�
	UPROPERTY(EditAnywhere, Category = Spawn, Meta = (AllowPrivateAccess = true))
	float EnemySpawnTime;

	UPROPERTY(EditAnywhere, Category = Spawn, Meta = (AllowPrivateAccess = true))
	float ItemBoxSpawnTime;
	
	FTimerHandle SpawnNPCTimerHandle = {};
	FTimerHandle SpawnItemBoxTimerHandle = {};

	// ���Ͱ� ���ŵ� �� ȣ��Ǵ� �Լ�
	UFUNCTION()
	void OnKeyNPCDestroyed(AActor* DestroyedActor);
};
