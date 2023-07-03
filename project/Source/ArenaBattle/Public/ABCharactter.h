// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "ABPlayerController.h"
#include "ABAnimInstance.h"
#include "DrawDebugHelpers.h"
#include "ABWeapon.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Components/WidgetComponent.h"
#include "ABCharactter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class ARENABATTLE_API AABCharactter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharactter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	// class�� ������ ������ ��ȯ�� ������� �ʴ´�.
	enum class EControlMode
	{
		TPS,
		QUARTERVIEW,
		NPC
	};

	// 0�� ��忡 ���������� Ȳ���� 3��Ī ����
	//void SetControlMode(int32 ControlMode);
	void SetControlMode(EControlMode NewControlMode);
	EControlMode CurrentControlMode = EControlMode::TPS;
	FVector DirectionToMove = FVector::ZeroVector;

	// ī�޶� ��ȯ �� �ε巴�� ��ȯ�ǵ��� ����
	float ArmLengthTo = 0.f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.f;
	float ArmRotationSpeed = 0.f;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ī�޶� ����
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	// input mapping context
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* View;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AttackAction;

	// DefaultsOnly �� �������Ʈ ����ȭ�鿡���� ��������
	// InstanceOnly �� ����Ʈ������ ��������
	// ���� ���� ����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	// ���� ���� ������
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	/*
	// ���� ����
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;
	*/

	// ĳ���Ͱ� ���⸦ �������ִ��� Ȯ��
	bool CanSetWeapon();
	// ĳ���Ͱ� ���⸦ �Ȱ����� �մٸ� ���Ͽ� ���⸦ ������Ű�� ������ �����ڸ� ĳ���ͷ� �����Ѵ�.
	void SetWeapon(class AABWeapon* NewWeapon);

	// ���� ĳ���Ͱ� ������ �ִ� ���� ����
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AABWeapon* CurrentWeapon;
	
	// ĳ���� ���� ������ ���� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UABCharacterStatComponent* CharacterStat;

	// UI ����
	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* HPBarWidget;

	// ���� �Լ�
	void Attack();
	// Attack Task���� ����� �� �ֵ��� Delegate ����
	FOnAttackEndDelegate OnAttackEnd;

	bool IsDead() {
		return ABAnim->GetDead();
	}

	// ĳ���� state machine
	void SetCharacterState(ECharacterState NewState);
	ECharacterState GetCharacterState() const;

	int32 GetExp() const;

	float GetFianlAttackRange() const;
	float GetFianlAttackDamage() const;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void UpDown(const float& NewAxisValue);
	void LeftRight(const float& NewAxisValue);
	void LookUp(const float& NewAxisValue);
	void Turn(const float& NewAxisValue);
	virtual void Jump() override;
	void ViewChange();
	bool moving = false;

	// ��Ÿ�� ���� ������ ��ũ�ϴ� �Լ�
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// ���� ���� ������ ���¸� Ȯ��
	void AttackStartComboState();
	// ���� ���� ������ ���¸� Ȯ��
	void AttackEndComboState();
	// ������ üũ���ִ� �Լ�
	void AttackCheck();
	/* 
		���� ����� ó���ϴ� �Լ�
		1. DamageAmount : ������ ������� ����
		2. DamageEvent : ����� ����
		3. EventInstigator : ���� ����� ���� ������
		4. DamageCauser : ����� ������ ���� ����� ���� 
	*/
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;


	// ���������� Ȯ���ϴ� ����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	bool IsAttacking;

	// ���� �ܰ� ������ �������� Ȯ���ϴ� ����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	bool CanNextCombo;

	// ���� �ܰ� ������ �Է��� �ִ��� Ȯ���ϴ� ����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	bool IsComboInputOn;

	// ���� �޺� ��
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	int32 CurrentCombo;

	// �ִ� �޺� ��
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	int32 MaxCombo;

	// �ִ� �ν��Ͻ�
	UPROPERTY()
	class UABAnimInstance* ABAnim;

	// �÷��̾ ��Ʈ���ϴ� ĳ���Ͱ� �ƴ� ��� NPC ��Ʈ�Ѹ�� ����ϵ��� ����
	//virtual void PossessedBy(AController* NewController) override; state���� �� ������ �ϰ� �ִ�

	// INI ���Ͽ��� �ּ� ��θ� �о ĳ���� �ּ� ��������
	void OnAssetLoadCompleted();
	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;

	int32 AssetIndex = 0;

	// ĳ���� state machine
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	ECharacterState CurrentState;

	// �÷��̾����� AI���� ������ ���� ����
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool bIsPlayer;

	UPROPERTY()
	class AABAIController* ABAIController;

	UPROPERTY()
	AABPlayerController* ABPlayerController;

	// ����� ���� Ÿ�̸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
	float DeadTimer;

	FTimerHandle DeadTimerHandle = {};
};
