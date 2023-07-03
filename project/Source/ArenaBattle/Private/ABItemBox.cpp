// Fill out your copyright notice in the Description page of Project Settings.


#include "ABItemBox.h"
#include "ABWeapon.h"
#include "ABCharactter.h"

// Sets default values
AABItemBox::AABItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));


	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	// �ڽ� �ݸ��� ������Ʈ�� Extend ���� ��ü �ڽ� ���� ũ���� ���� ���̴�.
	Trigger->SetBoxExtent(FVector(40.f, 42.f, 30.f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1"));
	if (SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Object);
	}

	// ����Ʈ ����
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
	if (P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		// ����Ʈ�� ���� ������ ������� �ʵ��� �����Ѵ�.
		Effect->bAutoActivate = false;
	}

	Box->SetRelativeLocation(FVector(0.f, -3.5f, -30.f));

	// ������Ʈ �ݸ��� ä��
	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	// ����ƽ �޽� �浹 ����
	Box->SetCollisionProfileName(TEXT("NoCollision"));

	// TSubclassOf�� �⺻ Ŭ���� ���� �����Ѵ�.
	WeaponItemClass = AABWeapon::StaticClass();
}

// Called when the game starts or when spawned
void AABItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnCharacterOverlap);
}

// Called every frame
void AABItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherOBdyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABLOG_S(Warning);

	// ABCharacter�� ĳ��Ʈ�ؼ� ����Ű��
	auto ABCharacter = Cast<AABCharactter>(OtherActor);
	ABCHECK(nullptr != ABCharacter);

	if (nullptr != ABCharacter && nullptr != WeaponItemClass)
	{
		if (ABCharacter->CanSetWeapon())
		{
			// ���忡 ���Ӱ� ���͸� �����ϴ� �Լ�. (���ڷ� ���Ͱ� ������ ������ ��ġ �� ȸ���� �����Ѵ�)
			auto NewWeapon = GetWorld()->SpawnActor<AABWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
			// ĳ���Ϳ� ���� ����
			ABCharacter->SetWeapon(NewWeapon);

			// ĳ���Ͱ� ���ڿ��� ���⸦ ���� �� ����Ʈ ���
			Effect->Activate(true);
			// �������� ������ ���ʹ� �����ش�.
			// ������ ���� �۾��� �� ���� �����ֵ�, �����÷��� �߿��� �������.
			Box->SetHiddenInGame(true, true);
			// �浹�� �������� ���̻� ��ȣ�ۿ����� �ʵ��� �Ѵ�.
			SetActorEnableCollision(false);
			Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);
		}
		else
		{
			ABLOG(Warning, TEXT("%s can't equip weapon currently."), *ABCharacter->GetName());
		}
	}
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent* PSystem)
{
	// ���� �ı�, ���͸� �����ش�.
	Destroy();
}

