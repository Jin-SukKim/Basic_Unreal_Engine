// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABItemBox.generated.h"

UCLASS()
class ARENABATTLE_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItemBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = Box)
	UBoxComponent* Trigger;
	
	UPROPERTY(VisibleAnywhere, Category = Box)
	UStaticMeshComponent* Box;

	// TSubclassOf�� Ư�� Ŭ������ ��ӹ��� Ŭ������� ����� ������ �� �ִ�.
	// �� ��� UClass(�𸮾� ������Ʈ)�� ���̴°� �ƴ� ������ ���ڿ� �̸� ������
	// Ŭ���� ����� ���� Ŭ������ �� �� �ִ�.
	UPROPERTY(EditInstanceOnly, Category = Box)
	TSubclassOf<class AABWeapon> WeaponItemClass;

	// ����Ʈ
	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent* Effect;

private:

	// �浹 �� ȣ��Ǵ� �Լ�, ��������Ʈ�� ����ϹǷ� UFUNCTION ��ũ�θ� ����Ѵ�.
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherOBdyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// ��ƼŬ ������Ʈ���� �����ϴ� ��������Ʈ�� ������ ����Ʈ ����� ����� ���Ͱ� ���ŵǵ��� �Ѵ�.
	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* PSystem);

};
