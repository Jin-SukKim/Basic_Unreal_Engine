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
	// 액터의 PostInitializeComponents가 호출되기 직전 호출되는 초기화 함수다.
	virtual void InitializeComponent() override;

public:	
	// 현재 프로젝트는 스탯이 변경될 때만 데이터를 처리할 예정이라 Tick 함수가 사용되지 않는다.
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 레벨 변경
	void SetNewLevel(int32 NewLevel);

	// 스탯 컴포넌트에서 대미지 처리
	void SetDamage(float NewDamage);
	float GetAttack();

	FOnHPIsZeroDelegate OnHPIsZero;

	// HP 변화
	void SetHP(float NewHP);
	float GetHPRatio();

	FOnHPChangeDelegate OnHPChanged;

	int32 GetDropExp() const;

private:
	// 데이터를 관리하는 변수들
	struct FABCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Level;

	// 게임 시작 시마다 변경되는 값의 보관은 디스크 낭비이므로 Transient 키워드로 직렬화(Serialization)에서 제외시켜준다.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;
		
};
