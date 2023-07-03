// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPawn.h"

// Sets default values
AABPawn::AABPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SRPINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	RootComponent = Capsule;
	Mesh->SetupAttachment(Capsule);
	SpringArm->SetupAttachment(Capsule);
	// ī�޶� springarm�� �ڽ����� ������ �������� ������Ʈ ���� �ɸ����Ѵ�..
	Camera->SetupAttachment(SpringArm);

	// �浹 ������Ʈ�� ĳ���� ũ�⿡ �°� ����
	Capsule->SetCapsuleHalfHeight(88.f);
	Capsule->SetCapsuleRadius(34.f);
	// ĳ���� ���ۿ� ���� ����Ʈ����� UE�� ��ǥ�谡 �ٸ��Ƿ� �� ��ǥ�踦 ���� ���ش�.
	// z���� -90�� ȸ������ �� ��ǥ�踦 �����ش�.
	// ���� ������ ���� ��ġ�� UE���� ���߾� ��ġ�� ����ϱ⿡
	// �ַ� �߹ٴ� ���� ��ġ(Pivot)�� ĳ���� �ּ��� ���� ���� ��ŭ �����ش�.
	Mesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	// ī�޶� �������� ���̴� 4���ͷ� �����ϰ�, ������Ʈ�� Y�� ȸ��(Pitch)�� -15���� ������
	// ��� �ʸӷ� ����� ���̵��� �����Ѵ�. (3��Ī)
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded())
	{
		Mesh->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Script/Engine.AnimBlueprint'/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C'"));
	//static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		Mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
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
}

// Called when the game starts or when spawned
void AABPawn::BeginPlay()
{
	Super::BeginPlay();

	// ��Ʈ�ѷ� ��������
	if (AABPlayerController* PlayerController = Cast<AABPlayerController>(GetController()))
	{	
		// enhanced input mapping context�� ���� ����ý���
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
}

// Called every frame
void AABPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AABPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// �Ķ���ͷ� UInputComponent�� �ޱ⿡ EnhancedInputComponent�� cast������Ѵ�.
	
	// UEnhancedInputComponent�� ����ϴ��� Ȯ���ؾ� �մϴ�. �׷��� ������ ������Ʈ�� ����� �������� �ʽ��ϴ�.
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Move()�� MoveAction�� InputAction�� bind���ش�.
		PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AABPawn::Move);
	}

	/*
	// UInputAction*�� �ڵ鷯 �Լ� �� ���� ����� ���� ETriggerEvent�� ���ε��ϴ� ����� ���� �����Դϴ�.
	PlayerEnhancedInputComponent->BindAction(TEXT("UpDown"), ETriggerEvent::Started, this, &AMyPawn::MyInputHandlerFunction);

	// �̵� Ű�� ��� ���� �� �� �Է� ������ �����Ǹ� ��� ƽ���� �̸����� �ڵ鷯 �Լ�(UFUNCTION)�� ȣ���մϴ�.
	PlayerEnhancedInputComponent->BindAction(TEXT("LeftRight"), ETriggerEvent::Triggered, this, TEXT("MyOtherInputHandlerFunction"));
	*/
}

void AABPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPawn::PossessedBy(AController* NewController)
{
	ABLOG_S(Warning);
	Super::PossessedBy(NewController);
}

void AABPawn::Move(const FInputActionValue& Value)
{
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
		//ABLOG(Warning, TEXT("%.3f %.3f"), current.X, current.Y);
	}
}

void AABPawn::UpDown(const float& Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void AABPawn::LeftRight(const float& Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}
