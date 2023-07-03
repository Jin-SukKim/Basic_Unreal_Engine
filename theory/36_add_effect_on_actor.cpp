/*
    액터에 이펙트를 부착한다.

    이펙트를 부착해 아이템 습득시 이펙트 재생을 하고 완료시 상자가 사라지는 기능을 구현한다.
    
    액터에 파티클 컴포넌트를 추가한 후, 해당 이펙트 애셋의 레퍼런스를 복사하고
    파티클 컴포넌트의 템플릿으로 이를 지정한다.

    멤버 함수를 하나 추가하고 파티클 컴포넌트 시스템에서 제공하는 OnSystemFinished 델리게이트에
    이를 연결해 이펙트 재생이 종료되면 아이템 상자가 제거되도록 로직을 구성한다.

    OnSystemFinished 델리게이트는 다이내믹 형식이므로 바인딩할 대상 멤버 함수에
    UFUNCTION 매크로를 선언해준다.

    이펙트 재생 시 액터의 충돌 기능을 제거해 캐릭터가 아이템을 두 번 습득하지 못하도록 방지하고,
    박스 스태틱메시도 액터가 제거될 때까지 모습ㅇ르 숨긴다.

    ItemBox.h
    ...
    // 이펙트
	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent* Effect;
    ...
    // 파티클 컴포넌트에서 제공하는 델리게이트를 연동해 이펙트 재생이 종료시 액터가 제거되도록 한다.
	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* PSystem);
    ...
    ItemBox.cpp
    ...생성자
    Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));
    Effect->SetupAttachment(RootComponent);

	// 이펙트 설정
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
	if (P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->bAutoActivate = false;
	}
    ...
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

    액터의 컴포넌트에서 시각적인 기능을 끄고자 할 때 사용하는 함수로 SetVisibility와 SetHiddeninGame이 있다.
    SetVisibility는 해당 컴포넌트의 시각적인 기능을 아예 없애는 함수다. (에디터 화면과 게임플레이 화면에서 모두 사라진다)
    HiddenInGame 옵션은 에디터 레벨 작업을 할 때는 보여주되, 게임플레이 중에는 사라진다.
*/