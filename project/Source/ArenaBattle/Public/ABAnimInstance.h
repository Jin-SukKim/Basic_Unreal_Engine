// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

// �Լ� �������� �ѹ��� ȣ�� ������ ��Ƽĳ��Ʈ ��������Ʈ ����
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
	// ƽ���� ȣ��Ǵ� �Լ�
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// ��Ÿ�� ���
	void PlayAttackMontage();
	// ���� ��Ÿ�� ������ �����ų �Լ�
	// GetAttackMontageSectionName�� �Բ� ����� ��Ÿ�� �̸��� �޴´�.(Attack1, Attack2 ��)
	void JumpToAttackMontageSection(int32 NewSection);

	// ��������Ʈ
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;

	// IsDead = true ����
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

	// DefaultsOnly �� �������Ʈ ����ȭ�鿡���� ��������
	// InstanceOnly �� ����Ʈ������ ��������
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	// ���� ������ �� �� �ֵ��� ��Ƽ���� ����
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();
	
	// ��Ÿ�� ������ �̸��� �����´�.
	FName GetAttackMontageSectionname(int32 Section);

	// �״� �ִϸ��̼��� ���� bool
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
};
