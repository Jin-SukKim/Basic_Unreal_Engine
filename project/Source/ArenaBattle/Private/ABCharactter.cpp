// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharactter.h"
#include "ABCharacterStatComponent.h"
#include "ABCharacterWidget.h"
#include "ABAIController.h"
#include "ABCharacterSetting.h"
#include "ABGameInstance.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABHUDWidget.h"
#include "ABGameMode.h"


// Sets default values
AABCharactter::AABCharactter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SRPINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	// ĸ�� ������Ʈ�� ��Ʈ ������Ʈ���״� �� �ؿ� �ٿ��ش�.
	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DEFAULT_CONTEXT(TEXT("/Game/Book/ABInputMappingContext.ABInputMappingContext"));
	if (DEFAULT_CONTEXT.Succeeded())
	{
		InputMapping = DEFAULT_CONTEXT.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MOVE_ACTION(TEXT("/Game/Book/ABInputAction.ABInputAction"));
	if (MOVE_ACTION.Succeeded())
	{
		MoveAction = MOVE_ACTION.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LOOK_ACTION(TEXT("/Game/Book/ABLook.ABLook"));
	if (LOOK_ACTION.Succeeded())
	{
		LookAction = LOOK_ACTION.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> VIEW_CHANGE(TEXT("/Game/Book/ABViewChange.ABViewChange"));
	if (VIEW_CHANGE.Succeeded())
	{
		View = VIEW_CHANGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JUMP_ACTION(TEXT("/Game/Book/ABJumpAction.ABJumpAction"));
	if (JUMP_ACTION.Succeeded())
	{
		JumpAction = JUMP_ACTION.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ATTACK_ACTION(TEXT("/Game/Book/ABAttackAction.ABAttackAction"));
	if (ATTACK_ACTION.Succeeded())
	{
		AttackAction = ATTACK_ACTION.Object;
	}

	/*
	// ���� ���� ����
	FName WeaponSocket(TEXT("hand_rSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight"));
		if (SK_WEAPON.Succeeded())
		{
			Weapon->SetSkeletalMesh(SK_WEAPON.Object);
		}

		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}
	*/

	SetControlMode(EControlMode::TPS);

	ArmLengthSpeed = 3.f;
	ArmRotationSpeed = 10.f;
	// GetCharacterMovement()�� ĳ���� �����Ʈ ������Ʈ�� ������ �� 420���� ������ �ִ�
	// JumpZVelocity�� �⺻���� �����Ѵ�.
	GetCharacterMovement()->JumpZVelocity = 800.f;

	// ��Ÿ�� �ִϸ��̼�
	IsAttacking = false;
	MaxCombo = 4;
	AttackEndComboState();

	// ĸ�� ������Ʈ�� ������ �������� ����ϵ��� �ڵ�� �⺻���� ����
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));
	// ���� ���� ����
	AttackRange = 80.f;
	AttackRadius = 50.f;

	// ĳ���� �Ӹ� ���� ������ ��ġ�� ����
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	// �׻� �÷��̾ ���� ������ Screen ���� �����Ѵ�.
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		// ������ ������ ũ�Ⱑ �°� 150x50���� ũ�� ����
		HPBarWidget->SetDrawSize(FVector2D(150.f, 50.f));
	}

	// ABCharacter�� AIController�� ������ ABAIController�� ����
	AIControllerClass = AABAIController::StaticClass();
	// �÷��̾ �����ϴ� ĳ���͸� ������ ������ ��ġ�ǰų� �����Ǵ� ��� ĳ���Ͱ� ABAIController�� ���踦 �޴´�.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// ArenaBattleSetting���� �߰��� �� ��⿡�� �ּ� ����� �о���δ�.
	/*
	auto DefaultSetting = GetDefault<UABCharacterSetting>();
	if (DefaultSetting->CharacterAssets.Num() > 0)
	{
		for (auto CharacterAsset : DefaultSetting->CharacterAssets)
		{
			ABLOG(Warning, TEXT("CHaracterAsset : %s"), *CharacterAsset.ToString());
		}
	}
	*/

	// �÷��̾��� ��� ĳ���� �ּ� ��� �� 4��° �ּ��� ����Ѵ�.
	AssetIndex = 4;

	// PREINIT state���� �⺻ ĳ���� �ּ��� ������ ������ ĳ���Ϳ� UI�� ���ܵΰ� ������� ���� �ʴ´�.
	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	SetCanBeDamaged(false);

	DeadTimer = 5.f;
}

// Called when the game starts or when spawned
void AABCharactter::BeginPlay()
{
	Super::BeginPlay();
	
	bIsPlayer = IsPlayerControlled();

	// �÷��̾����� NPC���� Ȯ��
	if (bIsPlayer)
	{
		// ��Ʈ�ѷ� ��������
		ABPlayerController = Cast<AABPlayerController>(GetController());
		if (ABPlayerController)
		{
			ABCHECK(nullptr != ABPlayerController);
			// enhancde input�� ����ý���
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ABPlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(InputMapping, 0);
			}
		}
	}
	else
	{
		ABAIController = Cast<AABAIController>(GetController());
		ABCHECK(nullptr != ABAIController);
	}

	// ArenaBattleSetting���� INI ������ �ּ� ��θ� �о�´�.
	auto DefaultSetting = GetDefault<UABCharacterSetting>();

	if (bIsPlayer)
	{
		auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
		ABCHECK(nullptr != ABPlayerState);
		AssetIndex = ABPlayerState->GetCharacterIndex();
	}
	else
	{
		// �ּ� ��Ͽ��� ��ε� �� �ϳ� �����ϰ� ��������
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}

	// Load�� ĳ���� �ּ�
	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);
	// AsyncLoad�� ���� �񵿱� ������� �ּ��� �ε�
	// �ּ��� �ε��� �Ϸ��ϸ� �ش� Delegate�� ����� �Լ��� ȣ�����ش�.
	// Delegate���� �����ϴ� CreateUObject ����� ����� �Ｎ���� Delegate�� ������ �Լ��� ���������ش�.
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad,
		FStreamableDelegate::CreateUObject(this, &AABCharactter::OnAssetLoadCompleted));
	// ������ ĳ���� �ּ��� �ε��ϴ� state�� ����
	SetCharacterState(ECharacterState::LOADING);
	/*
	// ĳ���� ������ ���Ͽ� ���� ����
	FName WeaponSocket(TEXT("hand_rSocket"));
	// ���忡 ���Ӱ� ���͸� �����ϴ� �Լ�. (���ڷ� ���Ͱ� ������ ������ ��ġ �� ȸ���� �����Ѵ�)
	auto Weapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (nullptr != Weapon)
	{
		// Attaches the RootComponent of this Actor to the supplied component, optionally at a named socket.
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}

	// ������ �ʱ�ȭ�� BeinPlaty���� �ʱ�ȭ �ȴ�.
	// UI ������ ĳ������ StatComponenet�� ����
	auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
	if (nullptr != CharacterWidget)
	{
		CharacterWidget->BindCharacterStat(CharacterStat);
	}
	*/
}

void AABCharactter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	// �ִ� �ν��Ͻ��� OnMontageEnded ��������Ʈ�� ������ OnAttackMontageEnded�� ������,
	// ��������Ʈ�� �ߵ��� ������ �ִϸ��̼� �ý��ۿ� ��Ÿ�� ��� ����� ������ ���ϰ� �� �������� �����ش�.
	ABCHECK(nullptr != ABAnim);

	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharactter::OnAttackMontageEnded);

	// NextAttackCheck ��������Ʈ�� ������ �����ϰ� ����
	// �޺��� �������� �ľ��ϰ� ��Ȳ�� ���� ����
	ABAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		//ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	// ������ üũ
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharactter::AttackCheck);

	// ������� �Ծ� HP�� 0���ϸ� �״� �ִϸ��̼� ��� �� �浹 ����
	CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	});
}


void AABCharactter::SetControlMode(EControlMode ControlMode)
{
	CurrentControlMode = ControlMode;

	switch(CurrentControlMode)
	{	
	case EControlMode::TPS:
		// ���������� ����
		//SpringArm->TargetArmLength = 450.f;
		ArmLengthTo = 450.f;
		// �θ� ���Ϳ��� ������� ���� (TPS�� ��� ���콺�� ���� �ޱ⿡ 0���� �ʱ�ȭ)
		//SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		// true�� ���(��������)�� �Է¿� ���� ȸ��
		SpringArm->bUsePawnControlRotation = true;
		// ���� pitch, yaw, roll �������� ȸ���� �������� Ȯ��
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritYaw = true;
		// ī�޶�� �÷��̾� ���̿� ��ü�� ���� �� ī�޶� ������ ������ ����
		SpringArm->bDoCollisionTest = true;
		// true�� ī�޶� ������ ���� ĳ���� �����̴� ����(Yaw) ��ȯ
		bUseControllerRotationYaw = false;
		//bOrientRotationToMovement true�� �Է¹޴� �������� ���� ȸ����(�� ������ ȸ������ ����)
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// ���� ȸ�� �ӵ� ����
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		// bUseControllerDesiredRotation true�� �Է¹޴� �������� ���� ȸ����(��Ʈ�ѷ��� ȸ������ ����)
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		break;
	case EControlMode::QUARTERVIEW:
		//SpringArm->TargetArmLength = 800.f;
		//SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
		ArmLengthTo = 800.f;
		ArmRotationTo = FRotator(-45.f, 0.f, 0.f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		break;
	case EControlMode::NPC:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
		break;
	}
}

// Called every frame
void AABCharactter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EControlMode::QUARTERVIEW:
		// ȸ�� ����
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));
		// vector�� ���� ������ �������� �����ϸ� ��� ���̱⿡ SizeSuqared�� �˻�
		if (DirectionToMove.SizeSquared() > 0.f)
		{
			// ���ͺ� ������ �׻� ������ ���� �ȴ�.
			// MakeFromX, Y, Z : �ϳ��� ���ͷκ��� ȸ�� ����� ����
			// �� ���� �Է��� �ջ��� ���� ���� ����� ĳ������ �ü� ����(X��)�� ��ġ�ؾ��� MakeFromX ���
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			if (moving)
				AddMovementInput(DirectionToMove);
			else
				AddMovementInput(DirectionToMove.ZeroVector);
		}
		moving = false;
		break;
	}
}

// Called to bind functionality to input
void AABCharactter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// UEnhancedInputComponent�� ����ϴ��� Ȯ���ؾ� �մϴ�. �׷��� ������ ������Ʈ�� ����� �������� �ʽ��ϴ�.
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Move()�� MoveAction�� InputAction�� bind���ش�.
		PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AABCharactter::Move);
		PlayerEnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AABCharactter::Look);
		PlayerEnhancedInputComponent->BindAction(View, ETriggerEvent::Started, this, &AABCharactter::ViewChange);
		// �θ�Ŭ������ Character�� �ִ� Jump�Լ��� ����Ѵ�.
		PlayerEnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AABCharactter::Jump);
		PlayerEnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AABCharactter::Attack);
	}

}


bool AABCharactter::CanSetWeapon()
{
	return true;
}

void AABCharactter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(nullptr != NewWeapon);

	// ���⸦ ������ �ִٸ� �� ���⸦ ������� �ִ°� �����ش�.
	if (nullptr != CurrentWeapon)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	// ����
	FName WeaponSocek(TEXT("hand_rSocket"));
	if (nullptr != NewWeapon)
	{
		// ���� ������ �����ڸ� ĳ���ͷ� �Ű��ش�.
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocek);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

void AABCharactter::SetCharacterState(ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;
	
	switch (CurrentState)
	{
	// LOADING state : ������ ĳ���� �ּ��� �ε��ϴ� state
	case ECharacterState::LOADING:
	{
		if (bIsPlayer)
		{	
			// LOADING state�̹Ƿ� �Է� ��Ȱ��ȭ
			DisableInput(ABPlayerController);

			// HUD������ ĳ���� ���� ������Ʈ�� ����
			ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

			auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
			ABCHECK(nullptr != ABPlayerState);
			CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());
		}
		// NPC�� ��� ���ھ�� ���� ����
		else
		{
			// GameMode���� ������ �����´�.
			auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
			ABCHECK(nullptr != ABGameMode);
			// ���� * 0.8�� �� ���� ����
			int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f));
			// �ּ� 1���� �ִ� 20 ����
			int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
			ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
			CharacterStat->SetNewLevel(FinalLevel);
		}
		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);
		break;
	}
	// READY state : ĳ���� �ּ� �ε��� �Ϸ�� state
	case ECharacterState::READY:
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		SetCanBeDamaged(true);

		// HP�� 0 ���ϸ� DEAD state�� ����
		CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
			SetCharacterState(ECharacterState::DEAD);
			}
		);

		// ������ �ʱ�ȭ
		auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != CharacterWidget);
		// UI ������ ĳ������ StatComponenet�� ����
		CharacterWidget->BindCharacterStat(CharacterStat);

		if (bIsPlayer)
		{
			// ���� ���� �� �Է� Ȱ��ȭ �� ī�޶� ����
			SetControlMode(EControlMode::QUARTERVIEW);
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
			EnableInput(ABPlayerController);

			/*
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			GetWorld()->SpawnActor<AABCharactter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator, SpawnParams);
			*/
		}
		else
		{
			SetControlMode(EControlMode::NPC);
			GetCharacterMovement()->MaxWalkSpeed = 400.f;
			// NPC�� ��� BehaviorTree Ȱ��ȭ
			ABAIController->RunAI();
		}
		break;
	}
	// DEAD state : ĳ���Ͱ� HP�� ������ ����� �� �߻��ϴ� state
	case ECharacterState::DEAD:
	{
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		ABAnim->SetDeadAnim();
		SetCanBeDamaged(false);

		if (bIsPlayer)
		{
			DisableInput(ABPlayerController);
		}
		else
		{
			ABAIController->StopAI();
		}

		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
			if (bIsPlayer)
			{
				// �÷��̾ ����ϸ� ���� �����
				//ABPlayerController->RestartLevel();

				// ���� ���� UI ����
				ABPlayerController->ShowResultUI();
			}
			else
			{
				// NPC�� ����ϸ� �������� ����
				Destroy();
			}
			}), DeadTimer, false
		);
		break;
	}
	}
}

ECharacterState AABCharactter::GetCharacterState() const
{
	return CurrentState;
}

int32 AABCharactter::GetExp() const
{
	return CharacterStat->GetDropExp();
}

float AABCharactter::GetFianlAttackRange() const
{
	return (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackRange() : AttackRange;
}

float AABCharactter::GetFianlAttackDamage() const
{
	float AttackDamage = (nullptr != CurrentWeapon) ? (CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage())
		: CharacterStat->GetAttack();
	float AttackModifier = (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackModifier() : 1.f;
	return AttackDamage * AttackModifier;
}

void AABCharactter::Move(const FInputActionValue& Value)
{
	if (!IsAttacking) {
		const FVector2D current = Value.Get<FVector2D>();
		if (!current.IsZero())
		{
			if (current.X)
			{
				LeftRight(current.X);
			}

			if (current.Y)
			{
				UpDown(current.Y);
			}
			moving = true;
			//ABLOG(Warning, TEXT("%.3f %.3f"), current.X, current.Y);
		}
	}
}

void AABCharactter::Look(const FInputActionValue& Value)
{
	const FVector2D mouse = Value.Get<FVector2D>();
	if (!mouse.IsZero())
	{
		if (mouse.X)
		{
			Turn(-mouse.X);
		}

		if (mouse.Y)
		{
			LookUp(mouse.Y);
		}
	}
}


void AABCharactter::UpDown(const float& Value)
{
	// AddMovementInput(GetActorForwardVector(), Value);
	// AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), Value);
	switch (CurrentControlMode)
	{
	case EControlMode::TPS:
		// FRotationMatrix�� ȸ���� ��ǥ�� ������ �����ϴ� ���
		// FRotator ������ ȸ�� ����� �����ϰ� �̸� ���� ��ȯ�� ��ǥ���� X, Y�� ������ �����´�.
		AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X), Value);
		break;
	case EControlMode::QUARTERVIEW:
		DirectionToMove.X = Value;
		break;
	}
	
}

void AABCharactter::LeftRight(const float& Value)
{
	//AddMovementInput(GetActorRightVector(), Value);
	//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), Value);
	switch (CurrentControlMode)
	{
	case EControlMode::TPS:
		AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y), Value);
		break;
	case EControlMode::QUARTERVIEW:
		DirectionToMove.Y = Value;
		break;
	}
}

void AABCharactter::LookUp(const float& Value)
{
	//AddControllerPitchInput(Value);
	switch (CurrentControlMode)
	{
	case EControlMode::TPS:
		AddControllerPitchInput(Value);
		break;
	}
}

void AABCharactter::Turn(const float& Value)
{
	//AddControllerYawInput(Value);
	switch (CurrentControlMode)
	{
	case EControlMode::TPS:
		AddControllerYawInput(Value);
		break;
	}
}

void AABCharactter::Jump()
{
	if (!IsAttacking)
		Super::Jump();
}

void AABCharactter::Attack()
{
	if (IsAttacking)
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AABCharactter::ViewChange()
{
	switch (CurrentControlMode)
	{
	case EControlMode::TPS:
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::QUARTERVIEW);
		break;
	case EControlMode::QUARTERVIEW:
		GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
		SetControlMode(EControlMode::TPS);
		break;
	}
}

void AABCharactter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	// ���� �ִϸ��̼��� ������ Task�� ������ �ϹǷ� Delegate�� �˷��ش�.
	OnAttackEnd.Broadcast();
}

void AABCharactter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AABCharactter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AABCharactter::AttackCheck()
{
	float FianlAttackRange = GetFianlAttackRange();

	FHitResult HitResult;
	// Ž�� ������� ���� ����� ���̴� �ڽ��� Ž���� �������� �ʵ��� ������ this�� �ְ�
	// Ž�� ���� ������ ����ü�� �⺻���� ����Ѵ�.
	// ���� : TraceTag�� �̸�(NAME_None = �̸��� ���ٰ� ���),
	// false = ������ �˻簡 �ƴ� ������ �˻縦 ����
	// (true = complex collision test, ������ �˻��
	// ����Ǵ� ù��° ������Ʈ, �浹 ���� �� ���� ������ ��Եȴ�.)
	// this = �浹�˻縦 �ϴ� ��ü�̸� �˻�ÿ� ���ܵȴ�.
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,  // ������ �浹�� Ž���� ��� �⵹�� ���Ϳ� ���õ� ������ ������� ����ü
		GetActorLocation(), // ������ Ž�� ������ ���� ������ ���Ͱ� �ִ� ��
		GetActorLocation() + GetActorForwardVector() * FianlAttackRange, // ������ Ž�� ������ �� ������ ���� �ü� �������� 200cm ��������
		FQuat::Identity,    // ȸ������ �⺻���� �����Ѵ�.
		ECollisionChannel::ECC_GameTraceChannel2, // ���� �浹 ������ ����� Ʈ���̽� ä�� ����(Attack ä��)
		FCollisionShape::MakeSphere(AttackRadius), // Ž���� ����� ������ ����(50cm �������� ��ü)
		Params  // Ž�� ���� �������� ����ü�� �⺻���� ���(Ž�� ���)
	);

#if ENABLE_DRAW_DEBUG

	// Ž�� ���ۿ��� �� �������� ���ϴ� ����
	FVector TraceVec = GetActorForwardVector() * FianlAttackRange;
	// ������ ���� ��ġ
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	// ���� ������ ����
	float HalfHeight = FianlAttackRange * 0.5f + AttackRadius;
	// ĸ���� ĳ���� �ü� �������� ���� ���ݹ����� �°� �������ش�.
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	// ���ݿ� �����ϸ� �ʷϻ� �����ϸ� ���������� ����
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 3.f;

	// ĸ����� �׷��ֱ�
	DrawDebugCapsule(
		GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);


#endif

	if (bResult)
	{
		if (HitResult.GetActor()->IsValidLowLevel())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName());

			FDamageEvent DamageEvent;
			// ����� ����
			// ������� ������ ����� ������ �÷��̾� ��Ʈ�ѷ��̹Ƿ� ��Ʈ�ѷ� ������ �����ش�.
			// �������� �ִ� ������� �ٲ�Ƿ� StatComponent���� ����� ���� �����´�.
			HitResult.GetActor()->TakeDamage(GetFianlAttackDamage(), DamageEvent, GetController(), this);
		}
	}
}

float AABCharactter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	/*
	if (FinalDamage > 0.f)
	{
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	}
	*/

	// StatComponent Ŭ������ ����� ����� ���
	CharacterStat->SetDamage(FinalDamage);

	if (CurrentState == ECharacterState::DEAD)
	{
		// ����� framework���� PlayerController�� ������ ������(Instigator) ���ڷ� ����
		if (EventInstigator->IsPlayerController())
		{
			auto PlayerController = Cast<AABPlayerController>(EventInstigator);
			ABCHECK(nullptr != PlayerController, 0.f);
			PlayerController->NPCKill(this);
		}
	}
	return FinalDamage;
}

/*
void AABCharactter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsPlayerControlled())
	{
		SetControlMode(EControlMode::TPS);
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	else
	{
		SetControlMode(EControlMode::NPC);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
}
*/

void AABCharactter::OnAssetLoadCompleted()
{
	// ���̷��� �޽ÿ� �ҷ��� �ּ� ����
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	// �ڵ鷯 �ʱ�ȭ
	AssetStreamingHandle.Reset();
	ABCHECK(nullptr != AssetLoaded);
	// �޽� ����
	GetMesh()->SetSkeletalMesh(AssetLoaded);
	// �ε��� ������ state�� READY�� ������ ���� ����
	SetCharacterState(ECharacterState::READY);
}

