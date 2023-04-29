/*
    무기 액터의 제작

    액터에 고정으로 무기를 장착하지 않고 필요에 따라 무기를 바꿀 수 있게 할며ㅕㄴ
    액터로 분리해 만드는 것이 좋다.

    지금은 캐릭터가 드는 무기는 실제로 충돌을 발생시키지 않고 플레이어의 액세서리로만 사용했다
    그래서 무기 액터의 루트 컴포넌트인 스켈레탈 메시 컴포넌트의 충돌 설정을
    NoCollision으로 지정한다.

    ABWeapon.h
    ...
    UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;
    ...
    ABWeapon.cpp
    // Sets default values
    AABWeapon::AABWeapon()
    {
        // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
        PrimaryActorTick.bCanEverTick = false;

        Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
        RootComponent = Weapon;

        static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight"));
        if (SK_WEAPON.Succeeded())
        {
            Weapon->SetSkeletalMesh(SK_WEAPON.Object);
        }

        Weapon->SetCollisionProfileName(TEXT("NoCollision"));
    }

    무기 액터를 제작했으면 이제 캐릭터가 이를 손에 집도록 기능을 추가한다.
    BeginPlay에서 무기 액터를 생성하고 이를 캐릭터에 부착시키는 코드를 추가한다.

    월드에서 새롭게 액터를 생성하는 명령은 SpwanActor다.
    액터는 월드에 존재하는 물체이므로 이는 월드의 명령어다.
    GetWorld 함수로 월드의 포인터를 가져와 SpawnActor 함수를 실행한다.
    인자에는 생성할 액터의 클래스와 액터가 앞으로 생성할 위치 및 회전을 지정한다.

    ABCharacter.cpp
    ...
    // Called when the game starts or when spawned
    void AABCharactter::BeginPlay()
    {
        ...
        FName WeaponSocket(TEXT("hand_rSocket"));
        auto Weapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
        if (nullptr != Weapon)
        {
            Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
        }
    }

    이전과 결과는 같지만 월드 아웃라이너에서 캐릭터가 들고 있는 무기는 액터로 분리돼 있다는 점이 다르다.
*/