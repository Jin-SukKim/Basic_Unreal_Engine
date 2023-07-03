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
	// 카메라를 springarm의 자식으로 설정해 스프링암 컴포넌트 끝에 걸리게한다..
	Camera->SetupAttachment(SpringArm);

	// 충돌 컴포넌트는 캐릭터 크기에 맞게 설정
	Capsule->SetCapsuleHalfHeight(88.f);
	Capsule->SetCapsuleRadius(34.f);
	// 캐릭터 제작에 사용된 소프트웨어와 UE의 좌표계가 다르므로 두 좌표계를 같게 해준다.
	// z축을 -90도 회전시켜 두 좌표계를 맞춰준다.
	// 또한 액터의 기준 위치를 UE에서 정중앙 위치를 사용하기에
	// 주로 발바닥 기준 위치(Pivot)인 캐릭터 애셋의 절반 높이 만큼 내려준다.
	Mesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	// 카메라 지지대의 길이는 4미터로 설정하고, 컴포넌트의 Y축 회전(Pitch)을 -15도로 설정해
	// 어깨 너머로 배경이 보이도록 설정한다. (3인칭)
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

	// 컨트롤러 가져오기
	if (AABPlayerController* PlayerController = Cast<AABPlayerController>(GetController()))
	{	
		// enhanced input mapping context를 위한 서브시스템
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

	// 파라미터로 UInputComponent를 받기에 EnhancedInputComponent로 cast해줘야한다.
	
	// UEnhancedInputComponent를 사용하는지 확인해야 합니다. 그러지 않으면 프로젝트가 제대로 설정되지 않습니다.
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Move()를 MoveAction인 InputAction에 bind해준다.
		PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AABPawn::Move);
	}

	/*
	// UInputAction*을 핸들러 함수 및 관심 대상인 각종 ETriggerEvent에 바인딩하는 방법은 여러 가지입니다.
	PlayerEnhancedInputComponent->BindAction(TEXT("UpDown"), ETriggerEvent::Started, this, &AMyPawn::MyInputHandlerFunction);

	// 이동 키를 길게 누를 때 등 입력 조건이 충족되면 모든 틱에서 이름으로 핸들러 함수(UFUNCTION)를 호출합니다.
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
