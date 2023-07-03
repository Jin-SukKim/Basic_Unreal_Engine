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

	// 캡슐 컴포넌트가 루트 컴포넌트일테니 이 밑에 붙여준다.
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
	// 소켓 본에 접근
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
	// GetCharacterMovement()로 캐릭터 무브먼트 컴포넌트를 가져온 후 420으로 설정돼 있는
	// JumpZVelocity의 기본값을 변경한다.
	GetCharacterMovement()->JumpZVelocity = 800.f;

	// 몽타주 애니메이션
	IsAttacking = false;
	MaxCombo = 4;
	AttackEndComboState();

	// 캡슐 컴포넌트가 생성한 프리셋을 사용하도록 코드로 기본값을 지정
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));
	// 공격 범위 설정
	AttackRange = 80.f;
	AttackRadius = 50.f;

	// 캐릭터 머리 위로 오도록 위치를 조정
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	// 항상 플레이어를 향해 보도록 Screen 모드로 지정한다.
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		// 생성한 위젯과 크기가 맞게 150x50으로 크기 조정
		HPBarWidget->SetDrawSize(FVector2D(150.f, 50.f));
	}

	// ABCharacter의 AIController를 생성한 ABAIController로 설정
	AIControllerClass = AABAIController::StaticClass();
	// 플레이어가 조종하는 캐릭터를 제외한 레벨에 배치되거나 생성되는 모든 캐릭터가 ABAIController의 지배를 받는다.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// ArenaBattleSetting으로 추가한 새 모듈에서 애셋 목록을 읽어들인다.
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

	// 플레이어인 경우 캐릭터 애셋 목록 중 4번째 애셋을 사용한다.
	AssetIndex = 4;

	// PREINIT state에선 기본 캐릭터 애셋이 설정돼 있지만 캐릭터와 UI를 숨겨두고 대미지를 입지 않는다.
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

	// 플레이어인지 NPC인지 확인
	if (bIsPlayer)
	{
		// 컨트롤러 가져오기
		ABPlayerController = Cast<AABPlayerController>(GetController());
		if (ABPlayerController)
		{
			ABCHECK(nullptr != ABPlayerController);
			// enhancde input의 서브시스템
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

	// ArenaBattleSetting에서 INI 파일의 애셋 경로를 읽어온다.
	auto DefaultSetting = GetDefault<UABCharacterSetting>();

	if (bIsPlayer)
	{
		auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
		ABCHECK(nullptr != ABPlayerState);
		AssetIndex = ABPlayerState->GetCharacterIndex();
	}
	else
	{
		// 애셋 목록에서 경로들 중 하나 랜덤하게 가져오기
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}

	// Load할 캐릭터 애셋
	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);
	// AsyncLoad를 통해 비동기 방식으로 애셋을 로딩
	// 애셋이 로딩을 완료하면 해당 Delegate에 연결된 함수를 호출해준다.
	// Delegate에서 제공하는 CreateUObject 명령을 사용해 즉석에서 Delegate를 생성해 함수와 연동시켜준다.
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad,
		FStreamableDelegate::CreateUObject(this, &AABCharactter::OnAssetLoadCompleted));
	// 선택한 캐릭터 애셋을 로딩하는 state로 변경
	SetCharacterState(ECharacterState::LOADING);
	/*
	// 캐릭터 생성시 소켓에 무기 장착
	FName WeaponSocket(TEXT("hand_rSocket"));
	// 월드에 새롭게 액터를 생성하는 함수. (인자로 액터가 앞으로 생성할 위치 및 회전을 지정한다)
	auto Weapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (nullptr != Weapon)
	{
		// Attaches the RootComponent of this Actor to the supplied component, optionally at a named socket.
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}

	// 위젯의 초기화는 BeinPlaty에서 초기화 된다.
	// UI 위젯을 캐릭터의 StatComponenet와 연동
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
	// 애님 인스턴스의 OnMontageEnded 델리게이트와 선언한 OnAttackMontageEnded를 연결해,
	// 델리게이트가 발동할 때까지 애니메이션 시스템에 몽타주 재생 명령을 내리지 못하게 폰 로직에서 막아준다.
	ABCHECK(nullptr != ABAnim);

	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharactter::OnAttackMontageEnded);

	// NextAttackCheck 델리게이트에 로직을 선언하고 구현
	// 콤보가 가능한지 파악하고 상황에 따라 대응
	ABAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		//ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	// 공격을 체크
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharactter::AttackCheck);

	// 대미지를 입어 HP가 0이하면 죽는 애니메이션 재생 및 충돌 금지
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
		// 스프링암의 길이
		//SpringArm->TargetArmLength = 450.f;
		ArmLengthTo = 450.f;
		// 부모 액터와의 상대적인 각도 (TPS의 경우 마우스로 제어 받기에 0으로 초기화)
		//SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		// true면 대상(스프링암)이 입력에 따라 회전
		SpringArm->bUsePawnControlRotation = true;
		// 각각 pitch, yaw, roll 방향으로 회전이 가능한지 확인
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritYaw = true;
		// 카메라와 플레이어 사이에 물체가 있을 때 카메라가 앞으로 오도록 설정
		SpringArm->bDoCollisionTest = true;
		// true면 카메라 시점에 따라 캐릭터 움직이는 방향(Yaw) 전환
		bUseControllerRotationYaw = false;
		//bOrientRotationToMovement true면 입력받는 방향으로 폰이 회전함(단 액터의 회전값을 받음)
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// 폰의 회전 속도 지정
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		// bUseControllerDesiredRotation true면 입력받는 방향으로 폰이 회전함(컨트롤러의 회전값을 받음)
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
		// 회전 보간
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));
		// vector의 값이 음수가 나오더라도 제곱하면 양수 값이기에 SizeSuqared로 검사
		if (DirectionToMove.SizeSquared() > 0.f)
		{
			// 쿼터뷰 시점은 항상 정면을 보게 된다.
			// MakeFromX, Y, Z : 하나의 벡터로부터 회전 행렬을 구축
			// 두 축의 입력을 합산한 최종 벡터 방향과 캐릭터의 시선 방향(X축)이 일치해야해 MakeFromX 사용
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
	// UEnhancedInputComponent를 사용하는지 확인해야 합니다. 그러지 않으면 프로젝트가 제대로 설정되지 않습니다.
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Move()를 MoveAction인 InputAction에 bind해준다.
		PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AABCharactter::Move);
		PlayerEnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AABCharactter::Look);
		PlayerEnhancedInputComponent->BindAction(View, ETriggerEvent::Started, this, &AABCharactter::ViewChange);
		// 부모클래스인 Character에 있는 Jump함수를 사용한다.
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

	// 무기를 가지고 있다면 새 무기를 얻기위해 있는걸 없애준다.
	if (nullptr != CurrentWeapon)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	// 소켓
	FName WeaponSocek(TEXT("hand_rSocket"));
	if (nullptr != NewWeapon)
	{
		// 무기 액터의 소유자를 캐릭터로 옮겨준다.
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
	// LOADING state : 선택한 캐릭터 애셋을 로딩하는 state
	case ECharacterState::LOADING:
	{
		if (bIsPlayer)
		{	
			// LOADING state이므로 입력 비활성화
			DisableInput(ABPlayerController);

			// HUD위젯과 캐릭터 스탯 컴포넌트를 연결
			ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

			auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
			ABCHECK(nullptr != ABPlayerState);
			CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());
		}
		// NPC의 경우 스코어로 레벨 조정
		else
		{
			// GameMode에서 점수를 가져온다.
			auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
			ABCHECK(nullptr != ABGameMode);
			// 점수 * 0.8을 해 레벨 조정
			int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f));
			// 최소 1에서 최대 20 레벨
			int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
			ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
			CharacterStat->SetNewLevel(FinalLevel);
		}
		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);
		break;
	}
	// READY state : 캐릭터 애셋 로딩이 완료된 state
	case ECharacterState::READY:
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		SetCanBeDamaged(true);

		// HP가 0 이하면 DEAD state로 변경
		CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
			SetCharacterState(ECharacterState::DEAD);
			}
		);

		// 위젯의 초기화
		auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != CharacterWidget);
		// UI 위젯을 캐릭터의 StatComponenet와 연동
		CharacterWidget->BindCharacterStat(CharacterStat);

		if (bIsPlayer)
		{
			// 게임 시작 시 입력 활성화 및 카메라 세팅
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
			// NPC인 경우 BehaviorTree 활성화
			ABAIController->RunAI();
		}
		break;
	}
	// DEAD state : 캐릭터가 HP를 소진해 사망할 때 발생하는 state
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
				// 플레이어가 사망하면 게임 재시작
				//ABPlayerController->RestartLevel();

				// 게임 종료 UI 띄우기
				ABPlayerController->ShowResultUI();
			}
			else
			{
				// NPC가 사망하면 레벨에서 퇴장
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
		// FRotationMatrix는 회전된 좌표계 정보를 저장하는 행렬
		// FRotator 값으로 회전 행렬을 생성하고 이를 토대로 변환된 좌표계의 X, Y축 방향을 가져온다.
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
	// 공격 애니메이션이 끝나면 Task가 끝나야 하므로 Delegate로 알려준다.
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
	// 탐색 방법으로 공격 명령을 내이는 자신은 탐색에 감지되지 않도록 포인터 this를 넣고
	// 탐색 반응 설정은 구조체의 기본값을 사용한다.
	// 인자 : TraceTag의 이름(NAME_None = 이름이 없다고 명시),
	// false = 복잡한 검사가 아닌 간단한 검사를 싱행
	// (true = complex collision test, 복잡한 검사로
	// 검출되는 첫번째 오브젝트, 충돌 지점 등 상세한 정보를 얻게된다.)
	// this = 충돌검사를 하는 객체이며 검사시에 제외된다.
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,  // 액터의 충돌이 탐지된 경우 출돌된 액터에 관련된 정보를 얻기위한 구조체
		GetActorLocation(), // 도형의 탐색 영역의 시작 지점은 액터가 있는 곳
		GetActorLocation() + GetActorForwardVector() * FianlAttackRange, // 도형의 탐색 영역의 끝 지점은 액터 시선 방향으로 200cm 떨어진곳
		FQuat::Identity,    // 회전값은 기본값을 지정한다.
		ECollisionChannel::ECC_GameTraceChannel2, // 물리 충돌 감지에 사용할 트레이스 채널 정보(Attack 채널)
		FCollisionShape::MakeSphere(AttackRadius), // 탐지에 사용할 도형을 제작(50cm 반지름의 구체)
		Params  // 탐색 반응 설정으로 구조체의 기본값을 사용(탐색 방법)
	);

#if ENABLE_DRAW_DEBUG

	// 탐색 시작에서 끝 지점까지 향하는 벡터
	FVector TraceVec = GetActorForwardVector() * FianlAttackRange;
	// 벡터의 중점 위치
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	// 벡터 길이의 절반
	float HalfHeight = FianlAttackRange * 0.5f + AttackRadius;
	// 캡슐을 캐릭터 시선 방향으로 눕혀 공격범위에 맞게 설정해준다.
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	// 공격에 성공하면 초록색 실패하면 빨간색으로 설정
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 3.f;

	// 캡슐모양 그려주기
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
			// 대미지 전달
			// 대미지는 폰에게 명령을 내리는 플레이어 컨트롤러이므로 컨트롤러 정보를 보내준다.
			// 레벨마다 주는 대미지가 바뀌므로 StatComponent에서 대미지 값을 가져온다.
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

	// StatComponent 클래스를 사용해 대미지 계산
	CharacterStat->SetDamage(FinalDamage);

	if (CurrentState == ECharacterState::DEAD)
	{
		// 대미지 framework에서 PlayerController의 정보는 가해자(Instigator) 인자로 전달
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
	// 스켈레톤 메시에 불러온 애셋 선언
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	// 핸들러 초기화
	AssetStreamingHandle.Reset();
	ABCHECK(nullptr != AssetLoaded);
	// 메시 세팅
	GetMesh()->SetSkeletalMesh(AssetLoaded);
	// 로딩이 끝나면 state를 READY로 변경해 게임 시작
	SetCharacterState(ECharacterState::READY);
}

