// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHPChangeDelegate);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// ������ PostInitializeComponents�� ȣ��Ǳ� ���� ȣ��Ǵ� �ʱ�ȭ �Լ���.
	virtual void InitializeComponent() override;

public:	
	// ���� ������Ʈ�� ������ ����� ���� �����͸� ó���� �����̶� Tick �Լ��� ������ �ʴ´�.
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ���� ����
	void SetNewLevel(int32 NewLevel);

	// ���� ������Ʈ���� ����� ó��
	void SetDamage(float NewDamage);
	float GetAttack();

	FOnHPIsZeroDelegate OnHPIsZero;

	// HP ��ȭ
	void SetHP(float NewHP);
	float GetHPRatio();

	FOnHPChangeDelegate OnHPChanged;

	int32 GetDropExp() const;

private:
	// �����͸� �����ϴ� ������
	struct FABCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Level;

	// ���� ���� �ø��� ����Ǵ� ���� ������ ��ũ �����̹Ƿ� Transient Ű����� ����ȭ(Serialization)���� ���ܽ����ش�.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;
		
};
