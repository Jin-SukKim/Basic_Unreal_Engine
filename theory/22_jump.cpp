/*
    점프 기능의 구현

    점프 액션을 만들어준다.

    // GetCharacterMovement()로 캐릭터 무브먼트 컴포넌트를 가져온 후 420으로 설정돼 있는
	// JumpZVelocity의 기본값을 변경한다.
	GetCharacterMovement()->JumpZVelocity = 800.f;
    로 원하는 높이를 설정한다.

    // 부모클래스인 Character에 있는 Jump함수를 사용한다.
	PlayerEnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AABCharactter::Jump);
    이렇게 점프 기능을 구현할 수 있따.
    
    점프 중에는 위아래로 향하는 속력이 발생하기에 캐릭터가 달리는 모션을 재생한다.
    이를 고쳐주기 위해선 애니메이션 블루프린트는 현재 캐릭터가 점프 중인지,
    또는 땅 위인지 파악할 수 있어야 한다.
    이 정보는 폰의 무브먼트 컴포넌트에서 얻어올 수 있다.

    현재 움직임을 파악하기 위해 폰의 무브먼트 컴포넌트가 제공하는 함수들

    1. IsFalling() : 현재 공중에 떠있는지 알려준다.
    2. IsSwimming() : 현재 수영 중인지 알려준다.
    3. IsCrouching() : 현재(쭈그리고) 앉아있는지 알려준다.
    4. IsMoveOnGround() : 땅 위에서 이동 중인지 알려준다.

    폰 무브먼트 모델에는 네 가지 지르이 함수가 설계돼 있지만, 실제로 이 기능을 제대로
    구현한 컴포넌트는 캐릭터가 사용하는 캐릭터 무브먼트 컴포넌트뿌니앋.

    FloatingPawnMovement 컴포넌트는 위 함수 모두 false 값을 반환한다.

    IsFalling() 함수로 점프 중인지 정보를 애님 인스턴스에 보관하고 이를 활용해 애니메이션을 재생한다.

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = ture))
	bool IsInAir;

    auto Character = Cast<ACharacter>(Pawn);
	if (Character)
	{
		IsInAir = Character->GetMovementComponent()->IsFalling();
	}

    그 후 애님 그래프의 스테이트 머신으로 돌아가 IsInAir 변수를 가지고 점프 스테이트를 만들어준다.

    정상적으로 만들어지면 애니메이션을 멈추고 T 인 기본 자세로 변하면서 점프하는 것을 볼 수 있따.

*/