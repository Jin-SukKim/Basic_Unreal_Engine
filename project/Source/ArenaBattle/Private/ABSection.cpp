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

	// �浹 ����
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

	// SM_SQAURE�� ����� ���Ͽ� ���� �޾��ش�.
	static FName GateSockets[] = { {TEXT("+XGate") }, {TEXT("-XGate") }, {TEXT("+YGate") }, {TEXT("-YGate") }};
	// static TArray<FName> GateSockets = Mesh->GetAllSocketNames();

	for (FName GateSocket : GateSockets)
	{
		// ������ �ִ��� Ȯ��
		ABCHECK(Mesh->DoesSocketExist(GateSocket));
		// ���Ͽ� �� �ּ� �������ֱ�
		UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString());
		NewGate->SetStaticMesh(SM_GATE.Object);
		NewGate->SetupAttachment(RootComponent, GateSocket);
		// �� �ּ��� pivot�� ���ʿ� �����־ Y�� -80.5f ��ŭ �������ش�.
		NewGate->SetRelativeLocation(FVector(0.f, -80.5f, 0.f));
		GateMeshes.Add(NewGate);

		// Collision ����
		UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*GateSocket.ToString().Append(TEXT("Trigger")));
		NewGateTrigger->SetBoxExtent(FVector(100.f, 100.f, 300.f));
		NewGateTrigger->SetupAttachment(RootComponent, GateSocket);
		NewGateTrigger->SetRelativeLocation(FVector(70.f, 0.f, 250.f));
		NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		GateTriggers.Add(NewGateTrigger);

		NewGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap);
		// � ����Ʈ�� ��������� �����ϱ����� Tag ����
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

		// ������ �ð��� ������ ����� �Լ��� �θ��� FTimerManager::SetTimer �Լ�
		// 2�� �Ŀ� NPC�� ����
		GetWorld()->GetTimerManager().SetTimer(
			SpawnNPCTimerHandle, // Timer �ڵ�
			FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), // �θ� �Լ�
			EnemySpawnTime, // ������ �ð�
			false // �ݺ��� ������
			//, float InFirstDelay // ���� Ÿ�̸��� ù �ݺ� �ð�(�Ƹ� ������?) �⺻ -1.f �� ����
		);

		// 5���Ŀ� NPC �ݰ� 6���� ���� ItemBox ����
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
	{	// ���� �������� �Ѿ �� ������ ����Ʈ�� ȸ������ ���� ������ ó�� ǥ���Ѵ�.
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
	
	// ����Ʈ�� ���� Tag Ȯ��
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	if (!Mesh->DoesSocketExist(SocketName))
		return;

	// Collision���� Overlap�� ������ ����Ʈ�� ��ġ
	FVector NewLocation = Mesh->GetSocketLocation(SocketName);

	TArray<FOverlapResult> OverlapResults;
	// Collision ���� ����
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);
	// ã�� ������Ʈ Ÿ��
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
	// ������ NPC
	auto KeyNPC = GetWorld()->SpawnActor<AABCharactter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator);
	if (nullptr != KeyNPC)
	{
		KeyNPC->OnDestroyed.AddDynamic(this, &AABSection::OnKeyNPCDestroyed);
	}
}

void AABSection::OnKeyNPCDestroyed(AActor* DestroyedActor)
{
	// �ı��� NPC �����´�.
	auto ABCharacter = Cast<AABCharactter>(DestroyedActor);
	ABCHECK(nullptr != ABCharacter);

	// LastHitBy �� ���������� ������� ���� Controller�� ������ ����� �ִ�.
	auto ABPlayerController = Cast<AABPlayerController>(ABCharacter->LastHitBy);
	ABCHECK(nullptr != ABPlayerController);

	// ���� ���� ��� ��������
	auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
	ABCHECK(nullptr != ABGameMode);
	ABGameMode->AddScore(ABPlayerController);

	// NPC�� �ı��Ǿ��⿡ ������ Complete�� ��ȯ���ش�.
	SetState(ESectionState::COMPLETE);
}
