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

	// 박스 콜리전 컴포넌트의 Extend 값은 전체 박스 영역 크기의 절반 값이다.
	Trigger->SetBoxExtent(FVector(40.f, 42.f, 30.f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1"));
	if (SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Object);
	}

	// 이펙트 설정
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
	if (P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		// 이펙트가 액터 생성시 재생되지 않도록 방지한다.
		Effect->bAutoActivate = false;
	}

	Box->SetRelativeLocation(FVector(0.f, -3.5f, -30.f));

	// 오브젝트 콜리전 채널
	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	// 스태틱 메시 충돌 설정
	Box->SetCollisionProfileName(TEXT("NoCollision"));

	// TSubclassOf의 기본 클래스 값을 지정한다.
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

	// ABCharacter를 캐스트해서 가리키기
	auto ABCharacter = Cast<AABCharactter>(OtherActor);
	ABCHECK(nullptr != ABCharacter);

	if (nullptr != ABCharacter && nullptr != WeaponItemClass)
	{
		if (ABCharacter->CanSetWeapon())
		{
			// 월드에 새롭게 액터를 생성하는 함수. (인자로 액터가 앞으로 생성할 위치 및 회전을 지정한다)
			auto NewWeapon = GetWorld()->SpawnActor<AABWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
			// 캐릭터에 무기 장착
			ABCharacter->SetWeapon(NewWeapon);

			// 캐릭터가 상자에서 무기를 얻을 때 이펙트 재생
			Effect->Activate(true);
			// 아이템을 얻으면 액터는 숨겨준다.
			// 에디터 레벨 작업을 할 때는 보여주되, 게임플레이 중에는 사라진다.
			Box->SetHiddenInGame(true, true);
			// 충돌을 해제시켜 더이상 상호작용하지 않도록 한다.
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
	// 액터 파괴, 액터를 없애준다.
	Destroy();
}

