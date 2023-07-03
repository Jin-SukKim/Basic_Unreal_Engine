// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

// 함수 여러개를 한번에 호출 가능한 멀티캐스트 델리게이트 선언
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABAnimInstance();
	// 틱마다 호출되는 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 몽타주 재생
	void PlayAttackMontage();
	// 다음 몽타주 색션을 실행시킬 함수
	// GetAttackMontageSectionName과 함께 사용해 몽타주 이름을 받는다.(Attack1, Attack2 등)
	void JumpToAttackMontageSection(int32 NewSection);

	// 델리게이트
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;

	// IsDead = true 설정
	void SetDeadAnim() {
		IsDead = true;
	}

	bool GetDead() {
		return IsDead;
	}

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = ture))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = ture))
	bool IsInAir;

	// DefaultsOnly 는 블루프린트 편집화면에서만 보여지고
	// InstanceOnly 는 뷰포트에서만 보여진다
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	// 공격 판정을 할 수 있도록 노티파이 설정
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();
	
	// 몽타주 섹션의 이름을 가져온다.
	FName GetAttackMontageSectionname(int32 Section);

	// 죽는 애니메이션을 위한 bool
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
};
