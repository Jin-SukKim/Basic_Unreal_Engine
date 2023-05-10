/*
    내비게이션 메시 시스템 설정

    섹션의 모든 스테이트 잘 구현됬으면 섹션에 NPC와 아이템 상자를 생성하는 기능을 추가한다.
    NPC와 아이템 상자가 생성될 시간을 지정할 속성을 추가하고 타이머 기능을 사용해 일정 시간 이후에
    이들을 생성한다.
    ABSection.h
    ...
	// NPC 생성
	void OnNPCSpawn();

	// 생성될 시간
	UPROPERTY(EditAnywhere, Category = Spawn, Meta = (AllowPrivateAccess = true))
	float EnemySpawnTime;

	UPROPERTY(EditAnywhere, Category = Spawn, Meta = (AllowPrivateAccess = true))
	float ItemBoxSpawnTime;
	
	FTimerHandle SpawnNPCTimerHandle = {};
	FTimerHandle SpawnItemBoxTimerHandle = {};
    ...
    ABSection.cpp
    ... 생성자
    EnemySpawnTime = 2.f;
	ItemBoxSpawnTime = 5.f;
    ...
    void AABSection::SetState(ESectionState NewState)
    {
    ...
    case ESectionState::BATTLE:
	{
        ...
        // 지정된 시간이 지나면 선언된 함수를 부르는 FTimerManager::SetTimer 함수
		GetWorld()->GetTimerManager().SetTimer(
			SpawnNPCTimerHandle, // Timer 핸들
			FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), // 부를 함수
			EnemySpawnTime, // 지정할 시간
			false // 반복할 것인지
			//, float InFirstDelay // 루핑 타이머의 첫 반복 시간(아마 딜레이?) 기본 -1.f 로 설정
		);

		GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
			FVector2D RandXY = FMath::RandPointInCircle(600.f);
			GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY, 30.f), FRotator::ZeroRotator);
			}), ItemBoxSpawnTime, false
        );
    ...
    void AABSection::OnNPCSpawn()
    {
        GetWorld()->SpawnActor<AABCharactter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator);
    }
    ...

    새로 생성된 NPC 캐릭터는 새로운 섹션에서 Navigation Mesh 영역이 설정되지 않았기 때문에
    움직이지 못하고 생성된 위치에 가만히 서있는다.
    새로 생성된 섹션 영역에도 내비게이션 메시가 만들어져야 NPC가 이를 활용해 플레이어로 이동할 수 있다.

    게임 실행 중에 동적으로 Navigation Mesh를 생성하도록 Project에 추가 설정을 해준다.
    Project Setting -> Navigation Mesh -> Runtime Generation 값을 Dynamic으로 변경해주면,
    새롭게 생성된 섹션에도 내비게이션 메시가 실시간으로 만들어져 적용된다.
*/