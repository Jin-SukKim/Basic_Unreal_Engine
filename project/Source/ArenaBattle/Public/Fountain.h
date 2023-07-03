// Fill out your copyright notice in the Description page of Project Settings.
// �м��� ����
#pragma once

#include "ArenaBattle.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Fountain.generated.h"

UCLASS()
class ARENABATTLE_API AFountain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// ���� �̺�Ʈ �Լ��� ȣ���ϴ��� ���캸�� ���� �α׸� ��´�.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �м���� �м��� �������� ��, ������ ���־�� �浹�� ����� ����ƽ�޽� ������Ʈ�� �����Ǿ��ִ�.

	// �����͸� �����ϸ� �޸𸮸� �������־�� ������ �𸮾� ������ ����ȯ��(Runtime)�� ����
	// ��ü�� ������ ������ �ڵ����� �޸𸮸� �Ҹ��Ű�� ����� �����Ѵ�.
	// UPROPERTY()��� ��ũ�θ� ����� ��ü�� �������ָ� ������ ��ü�� �ڵ����� �������ش�.
	// �𸮾� ������Ʈ��� Ư���� ��ü���� ����� �� �ִ�.
	UPROPERTY(VisibleAnywhere)	// ���� ����
	UStaticMeshComponent* Body;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Water;

	// ����
	UPROPERTY(VisibleAnywhere)
	UPointLightComponent* Light;

	// �����̴� ����Ʈ
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* Splash;

	// UPROPERTY ��ũ�� �������� 0�� �ʱ� ���� ����޴� ��� ����
	// EditAnywhere Ű����� ��� ������ �� �ְ� �Ѵ�.
	// Category=�з��� ��Ģ���� ������ �з��� �ð��� �����Ѵ�.
	UPROPERTY(EditAnywhere, Category=ID)
	int32 ID;

	// ȸ�� �����Ʈ ������Ʈ
	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* Movement;

private:
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	float RotateSpeed;
};
