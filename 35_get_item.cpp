/*
    아이템의 습득

    아이템 습득 시 빈손의 플레이어에게 아이템을 쥐어주는 기능을 구현한다.

    배치한 아이템 상자에 클래스 정보를 저장할 속성을 추가하고,
    이 값을 기반으로 아이템을 생성하고 습득하게 기능을 구현한다.

    클래스 정보를 저장한는 변수 선언 시 UClass 포인터 사용이 가능하지만,
    이를 사용하면 현재 프로젝트에 사용하는 모든 언리얼 오브젝트의 선언이 보이게 된다.

    UE은 특정 클래스와 상속받은 클래스들로 목록을 한정하도록 TSubclassof라는 키워드를 제공한다.
    
    이를 사용하면 목록에서 아이템 상자와 이를 선언한 클래스 목록만 볼 수 있다.

    ItemBox.h
    ...
    // TSubclassOf는 특정 클래스와 상속받은 클래스들로 목록을 한정할 수 있다.
	// 즉 모든 UClass(언리얼 오브젝트)가 보이는게 아닌 아이템 상자와 이를 선언한
	// 클래스 목록인 웨폰 클래스만 볼 수 있다.
	UPROPERTY(EditInstanceOnly, Category = Box)
	TSubclassOf<class AABWeapon> WeaponItemClass;
    ...
    ItemBox.cpp
    ... 생성자
    // TSubclassOf의 기본 클래스 값을 지정한다.
	WeaponItemClass = AABWeapon::StaticClass();
    ...
    
    이러면 상자의 detail 윈도우에 클래스를 지정하는 메뉴가 나타난다.
    선언한 Weapon 클래스가 들어가 있다.

    캐릭터에 무기를 장착시키는 SetWeapon 함수를 선언한다.
    여기서는 현재 캐릭터에 무기가 없으면 hand_rSocket에 장착시키고 무기 액터의 소유자를
    캐릭터로 변경한다.

    Character.h
    ...
    // 캐릭터가 무기를 가지고있는지 확인
	bool CanSetWeapon();
	// 캐릭터가 무기를 안가지고 잇다면 소켓에 무기를 장착시키고 액터의 소유자를 캐릭터로 변경한다.
	void SetWeapon(class AABWeapon* NewWeapon);
    // 현재 캐릭터가 가지고 있는 무기 액터
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AABWeapon* CurrentWeapon;
    ...
    Character.cpp
    ...
    bool AABCharactter::CanSetWeapon()
    {
        return (nullptr == CurrentWeapon);
    }

    void AABCharactter::SetWeapon(AABWeapon* NewWeapon)
    {
        ABCHECK(nullptr != NewWeapon && nullptr == CurrentWeapon);
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
    ...
    배치한 상자에 Overlap 이벤트 발생 시 아이템 상자에 설정된 클래스 정보로부터 무기를 생성하고 캐릭터에 장착시킨다.

    아이템 상자에서 다른 무기도 생성할 수 있도록 새로운 무기를 추가한다.
    새로운 무기를 생성하기 위해 매번 C++로 클래스를 새로 추가하는 작업은 불편하므로 
    블루프린트를 사용해 ABWeapon을 상속받은 객체를 다수 생성한다.

    Content Browser에서 우클릭 후 블루프린트 클래스를 선택해 생성한다.
    생성 시 하단의 All Class를 펼처 ABWeapon을 검색해 ABWeapon을 부모로 하는
    블루프린트를 생성하고 이름을 지정해준다.

    블루프린트가 생성되면 부모 클래스의 기본값을 이어받기 때문에 기본적으로 BlackKnight 검이 만들어져 있다.
    이 블루프린트를 열어준 뒤 우측 창에서 스켈레탈 메시 컴포넌트를 원하는 애셋으로 변경해준다.
    위 툴바에서 컴파일과 저장을 해준다.

    그 후 아이템 박스의 선언해줬던 Weapon Item Class를 생성한 블루프린트를 사용하도록 변경한다.
    그러면 검이 아닌 도끼가 주어진다.

*/