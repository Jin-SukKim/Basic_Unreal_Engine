/*
    캐릭터 소켓 설정

    무기는 캐릭터에 트랜스폼으로 배치하는 것이 아니라 메시에 착용해야
    캐릭터 애니메이션에 따라 무기가 움직인다.

    UE은 캐릭터에 무기나 액세서리를 부착하기 위한 용도로 소켓이라는 시스템을 제공한다.
    
    현재 사용중인 스켈레탈 메시를 열면 오른쪽 손에 이미 hand_rSocket이란 이름의 소켓이 생성돼 있다.
    이를 사용해 무기를 부착한다.

    캐릭터의 솤세에는 착용할 아이템을 미리 볼 수 있는 기능이 있다.
    skeletal tree -> hand_rSocket에 우클릭 -> 브리뷰 애셋 추가 -> 원하는 에셋 검색후 부착한다.
    (소켓을 추가하고 싶다면 원하는 bone에 우클릭해 소켓을 추가한다.)

    이건 말그대로 미리보기 기능이므로 실제로 캐릭터에 부착되는 것은 아니다.
    현재 사용하는 hand_rSocket의 트랜스폼이 정확하지 않아 무기의 위치가 부자연스럽다.
    
    자연스럽게 보이도록 위치를 재조정한다.
    그리고 무기를 부착한 캐릭터가 자연스럽게 애니메이션을 재생하도록 애니메이션을 조정한다.
    애니메이션은 툴바의 프리뷰 애니메이션으로 선택가능하다.

    현재 위치 값 (-8.706434, 4.657758, 2.196209), 회전 값 (-0.001039, 0.000802, 80.000531)을 사용하도록 한다.

    소켓 설정을 완료하면 이를 사용해 캐릭터에 무기를 부착한다.

    외부에서 가져온 무기 애셋은 모두 스켈레탈 메시이기에 스켈레탈 메시 컴포넌트를 캐릭터 메시에 부착한다.
    무기 애셋의 레퍼런스를 복사한 후 스켈레탄 메시 컴포넌트에 로딩하고 SetupAttachment 함수에
    소켓 이름을 인자로 넘기면 소켓 위치를 기준으로 트랜스폼이 자동으로 설정된다.

    ABCharacter.h
    ...
    // 무기 소켓
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;
    ...
    ABCharacter.cpp
    ...
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