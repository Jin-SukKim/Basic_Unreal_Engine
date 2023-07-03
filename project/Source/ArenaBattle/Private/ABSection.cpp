// Fill out your copyright notice in the Description page of Project Settings.


#include "ABSection.h"
#include "ABCharactter.h"
#include "ABItemBox.h"
#include "ABPlayerController.h"
#include "ABGameMode.h"

// Sets default values
AABSection::AABSection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;

	FString AssetPath = TEXT("/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SQUARE(*AssetPath);
	if (SM_SQUARE.Succeeded())
	{
		Mesh->SetStaticMesh(SM_SQUARE.Object);
	}
	else
	{
		ABLOG(Error, TEXT("Failed to load staticmesh asset : %s"), *AssetPath);
	}

	// 충돌 설정
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Trigger->SetBoxExtent(FVector(775.f, 775.f, 300.f));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector(0.f, 0.f, 250.f));
	Trigger->SetCollisionProfileName(TEXT("ABTrigger"));

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap);

	FString GateAssetPath = TEXT("/Game/Book/StaticMesh/SM_GATE.SM_GATE");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GATE(*GateAssetPath);
	if (!SM_GATE.Succeeded())
	{
		ABLOG(Error, TEXT("Failed to load staticmesh asset : %s"), *GateAssetPath);
	}

	// SM_SQAURE의 사방의 소켓에 문을 달아준다.
	static FName GateSockets[] = { {TEXT("+XGate") }, {TEXT("-XGate") }, {TEXT("+YGate") }, {TEXT("-YGate") }};
	// static TArray<FName> GateSockets = Mesh->GetAllSocketNames();

	for (FName GateSocket : GateSockets)
	{
		// 소켓이 있는지 확인
		ABCHECK(Mesh->DoesSocketExist(GateSocket));
		// 소켓에 문 애셋 부착해주기
		UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString());
		NewGate->SetStaticMesh(SM_GATE.Object);
		NewGate->SetupAttachment(RootComponent, GateSocket);
		// 문 애셋의 pivot이 왼쪽에 몰려있어서 Y축 -80.5f 만큼 조정해준다.
		NewGate->SetRelativeLocation(FVector(0.f, -80.5f, 0.f));
		GateMeshes.Add(NewGate);

		// Collision 설정
		UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*GateSocket.ToString().Append(TEXT("Trigger")));
		NewGateTrigger->SetBoxExtent(FVector(100.f, 100.f, 300.f));
		NewGateTrigger->SetupAttachment(RootComponent, GateSocket);
		NewGateTrigger->SetRelativeLocation(FVector(70.f, 0.f, 250.f));
		NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		GateTriggers.Add(NewGateTrigger);

		NewGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap);
		// 어떤 게이트에 감지됬는지 구분하기위해 Tag 설정
		NewGateTrigger->ComponentTags.Add(GateSocket);

	}

	bNoBattle = false;

	EnemySpawnTime = 2.f;
	ItemBoxSpawnTime = 5.f;
}

// Called when the game starts or when spawned
void AABSection::BeginPlay()
{
	Super::BeginPlay();
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

// Called every frame
void AABSection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABSection::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

void AABSection::SetState(ESectionState NewState)
{
	switch (NewState)
	{
	case ESectionState::READY:
	{
		Trigger->SetCollisionProfileName(TEXT("ABTrigger"));

		for (UBoxComponent* GateTrigger : GateTriggers)
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));

		OperateGates(true);

		break;
	}
	case ESectionState::BATTLE:
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));

		for (UBoxComponent* GateTrigger : GateTriggers)
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));

		OperateGates(false);

		// 지정된 시간이 지나면 선언된 함수를 부르는 FTimerManager::SetTimer 함수
		// 2초 후에 NPC가 생성
		GetWorld()->GetTimerManager().SetTimer(
			SpawnNPCTimerHandle, // Timer 핸들
			FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), // 부를 함수
			EnemySpawnTime, // 지정할 시간
			false // 반복할 것인지
			//, float InFirstDelay // 루핑 타이머의 첫 반복 시간(아마 딜레이?) 기본 -1.f 로 설정
		);

		// 5초후에 NPC 반경 6미터 내의 ItemBox 생성
		GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
			FVector2D RandXY = FMath::RandPointInCircle(600.f);
			GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY, 30.f), FRotator::ZeroRotator);
			}), ItemBoxSpawnTime, false
		);

		break;
	}
	case ESectionState::COMPLETE:
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));

		for (UBoxComponent* GateTrigger : GateTriggers)
			GateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));

		OperateGates(true);

		break;
	}
	}

	CurrentState = NewState;
}

void AABSection::OperateGates(bool bOpen)
{
	for (UStaticMeshComponent* Gate : GateMeshes)
	{	// 다음 섹션으로 넘어갈 수 있으면 게이트를 회전시켜 문이 열린것 처럼 표현한다.
		Gate->SetRelativeRotation(bOpen ? FRotator(0.f, -90.f, 0.f) : FRotator::ZeroRotator);
	}
}

void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABLOG(Warning, TEXT("Trigger"));
	if (CurrentState == ESectionState::READY)
	{
		SetState(ESectionState::BATTLE);
	}
}

void AABSection::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABCHECK(OverlappedComponent->ComponentTags.Num() == 1);
	
	// 게이트에 붙인 Tag 확인
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	if (!Mesh->DoesSocketExist(SocketName))
		return;

	// Collision에서 Overlap이 감지된 게이트의 위치
	FVector NewLocation = Mesh->GetSocketLocation(SocketName);

	TArray<FOverlapResult> OverlapResults;
	// Collision 반응 설정
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);
	// 찾는 오브젝트 타입
	FCollisionObjectQueryParams ObjectQueryParam(FCollisionObjectQueryParams::InitType::AllObjects);
	bool bResult = GetWorld()->OverlapMultiByObjectType
	(
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		ObjectQueryParam,
		FCollisionShape::MakeSphere(775.0f),
		CollisionQueryParam
	);

	if (!bResult)
	{
		auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator);
	}
	else
	{
		ABLOG(Warning, TEXT("New section area is not empty."));
	}
}

void AABSection::OnNPCSpawn()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnNPCTimerHandle);
	// 스폰된 NPC
	auto KeyNPC = GetWorld()->SpawnActor<AABCharactter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator);
	if (nullptr != KeyNPC)
	{
		KeyNPC->OnDestroyed.AddDynamic(this, &AABSection::OnKeyNPCDestroyed);
	}
}

void AABSection::OnKeyNPCDestroyed(AActor* DestroyedActor)
{
	// 파괴된 NPC 가져온다.
	auto ABCharacter = Cast<AABCharactter>(DestroyedActor);
	ABCHECK(nullptr != ABCharacter);

	// LastHitBy 는 마지막으로 대미지를 입힌 Controller의 정보가 저장되 있다.
	auto ABPlayerController = Cast<AABPlayerController>(ABCharacter->LastHitBy);
	ABCHECK(nullptr != ABPlayerController);

	// 현재 게임 모드 가져오기
	auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
	ABCHECK(nullptr != ABGameMode);
	ABGameMode->AddScore(ABPlayerController);

	// NPC가 파괴되었기에 섹션을 Complete로 변환해준다.
	SetState(ESectionState::COMPLETE);
}

