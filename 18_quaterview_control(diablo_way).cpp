/*
    디아블로 방식의 쿼터뷰 컨트롤 구현

    고정된 삼인칭 시점에서의 컨트롤을 구현한다.

    조작 방식

    1. 캐릭터의 이동 : 상하좌우 키를 조합해 캐릭터가 이동할 방향을 결저
    2. 캐릭터의 회전 : 캐릭터는 입력한 방향으로 회전
    3. 카메라 길이 : 조금 멀리 떨어진 800cm
    4. 카메라 회전 : 카메라의 회전 없이 항상 고정 시선으로 45도로 내려다봄
    5. 카메라 줌 : 없음. 카메라와 캐릭터 사이에 장애물이 있는 경우 외곽선으로 처리

    SpringArm->TargetArmLength = 800.f;
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bDoCollisionTest = false;
	bUseControllerRotationYaw = true;

    상하 키와 좌우 키를 각각 처리하는게 아닌 조합해 캐릭터의 회전과 이동이 이뤄져야 한다.
    이를 위해 각 축의 입력을 조합해 보관할 벡터 유형 변수도 추가한다.

    입력의 Axis 이벤트와 Tick 이벤트는 모두 매 프레임마다 호출된다.
    플레이어의 입력 값에 따라 액터의 행동을 결정하므로, 이 두 이벤트 함수는
    같은 프레임마다 호출되더라도 입력 함수를 먼저 호출해 플레이어의 의지를 확인하고
    그 뒤에 Tick 함수를 호출해 플레이어의 입력에 대응할 액터의 최종 행동을 결정한다.

    // Called every frame
    void AABCharactter::Tick(float DeltaTime)
    {
        Super::Tick(DeltaTime);

        switch (CurrentControlMode)
        {
        case EControlMode::DIABLO:
            // vector의 값이 음수가 나오더라도 제곱하면 양수 값이기에 SizeSuqared로 검사
            if (DirectionToMove.SizeSquared() > 0.f)
            {
                // 쿼터뷰 시점은 항상 정면을 보게 된다.
                GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
                AddMovementInput(DirectionToMove);
            }
        }
    }
        
    void AABCharactter::UpDown(const float& Value)
    {
        // AddMovementInput(GetActorForwardVector(), Value);
        // AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), Value);
        switch (CurrentControlMode)
        {
        case EControlMode::GTA:
            AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X), Value);
            break;
        case EControlMode::DIABLO:
            DirectionToMove.X = Value;
            break;
        }
        
    }

    void AABCharactter::LeftRight(const float& Value)
    {
        //AddMovementInput(GetActorRightVector(), Value);
        //AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), Value);
        switch (CurrentControlMode)
        {
        case EControlMode::GTA:
            AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y), Value);
            break;
        case EControlMode::DIABLO:
            DirectionToMove.Y = Value;
            break;
        }
    }

    void AABCharactter::LookUp(const float& Value)
    {
        //AddControllerPitchInput(Value);
        switch (CurrentControlMode)
        {
        case EControlMode::GTA:
            AddControllerPitchInput(Value);
            break;
        }
    }

    void AABCharactter::Turn(const float& Value)
    {
        //AddControllerYawInput(Value);
        switch (CurrentControlMode)
        {
        case EControlMode::GTA:
            AddControllerYawInput(Value);
            break;
        }
    }

    UE의 FRotationMatrix는 회전된 좌표계 정보를 저장하는 행렬이다.
    TPS에선 FRotator 값으로 회전 행렬을 생성하고 이를 토대로 변환된 좌표계의 X, Y축 방향을 가져왔다.
    쿼터뷰 형식은 거꾸로 하나의 벡터 값과 이에 직교하는 나머지 두 축을 구해 회전 행렬을 생성하고
    일치하는 FRotator 값을 얻어온다.
    MakeFromX, Y, Z가 하나의 벡터로부터 회전 행렬을 구축한다.
    두 축의 입력을 합산한 최종 벡터 방향과 캐릭터의 시선 방향(X축)이 일치해야 하므로
    MakeFromX가 사용됬다.

    InterpTo 명령어를 사용해 목표 설정 값으로 서서히 변경되는 보간이 가능하다.
    float형은 FInterpTo, Vector형은 VInterpTo, Rotator형은 RInterpTo를 FMath를 통해 제공한다.
*/