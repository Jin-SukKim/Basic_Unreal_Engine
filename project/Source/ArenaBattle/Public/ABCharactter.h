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

	// class로 선언해 묵시적 변환을 허용하지 않는다.
	enum class EControlMode
	{
		TPS,
		QUARTERVIEW,
		NPC
	};

	// 0번 모드에 스프링암을 황용한 3인칭 조작
	//void SetControlMode(int32 ControlMode);
	void SetControlMode(EControlMode NewControlMode);
	EControlMode CurrentControlMode = EControlMode::TPS;
	FVector DirectionToMove = FVector::ZeroVector;

	// 카메라 변환 시 부드럽게 변환되도록 보간
	float ArmLengthTo = 0.f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.f;
	float ArmRotationSpeed = 0.f;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 카메라 설정
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

	// DefaultsOnly 는 블루프린트 편집화면에서만 보여지고
	// InstanceOnly 는 뷰포트에서만 보여진다
	// 공격 범위 길이
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	// 공격 범위 반지름
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	/*
	// 무기 소켓
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;
	*/

	// 캐릭터가 무기를 가지고있는지 확인
	bool CanSetWeapon();
	// 캐릭터가 무기를 안가지고 잇다면 소켓에 무기를 장착시키고 액터의 소유자를 캐릭터로 변경한다.
	void SetWeapon(class AABWeapon* NewWeapon);

	// 현재 캐릭터가 가지고 있는 무기 액터
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AABWeapon* CurrentWeapon;
	
	// 캐릭터 스탯 관리용 액터 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UABCharacterStatComponent* CharacterStat;

	// UI 위젯
	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* HPBarWidget;

	// 공격 함수
	void Attack();
	// Attack Task에서 사용할 수 있도록 Delegate 선언
	FOnAttackEndDelegate OnAttackEnd;

	bool IsDead() {
		return ABAnim->GetDead();
	}

	// 캐릭터 state machine
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

	// 몽타주 종료 시점을 세크하는 함수
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 공격 시작 시점의 상태를 확인
	void AttackStartComboState();
	// 공격 종료 시점의 상태를 확인
	void AttackEndComboState();
	// 공격을 체크해주는 함수
	void AttackCheck();
	/* 
		받은 대미지 처리하는 함수
		1. DamageAmount : 전달할 대미지의 세기
		2. DamageEvent : 대미지 종류
		3. EventInstigator : 공격 명령을 내린 가해자
		4. DamageCauser : 대미지 전달을 위해 사용한 도구 
	*/
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;


	// 공격중인지 확인하는 변수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	bool IsAttacking;

	// 다음 단계 공격이 가능한지 확인하는 변수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	bool CanNextCombo;

	// 다음 단계 공격의 입력이 있는지 확인하는 변수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	bool IsComboInputOn;

	// 현재 콤보 수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	int32 CurrentCombo;

	// 최대 콤보 수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = ture))
	int32 MaxCombo;

	// 애님 인스턴스
	UPROPERTY()
	class UABAnimInstance* ABAnim;

	// 플레이어가 컨트롤하는 캐릭터가 아닌 경우 NPC 컨트롤모드 사용하도록 설정
	//virtual void PossessedBy(AController* NewController) override; state에서 이 역할을 하고 있다

	// INI 파일에서 애셋 경로를 읽어서 캐릭터 애셋 가져오기
	void OnAssetLoadCompleted();
	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;

	int32 AssetIndex = 0;

	// 캐릭터 state machine
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	ECharacterState CurrentState;

	// 플레이어인지 AI인지 구분을 위한 변수
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool bIsPlayer;

	UPROPERTY()
	class AABAIController* ABAIController;

	UPROPERTY()
	AABPlayerController* ABPlayerController;

	// 사망한 이후 타이머
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
	float DeadTimer;

	FTimerHandle DeadTimerHandle = {};
};
