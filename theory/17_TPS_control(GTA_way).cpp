/*
    삼인칭 컨트롤

    3인칭 프로젝트의 기본 마네킹과 동일한 방식으로 컨트롤을 구현해본다.

    블루프린트로 제작한 흰색 마네킹의 설정

    1. 캐릭터 이동 : 현재 보는 시점을 기준으로 상하, 좌우 방향으로 마네킹이 이동하고 카메라는 회전하지 않느다.
    2. 캐릭터의 회전: 캐릭터가 이동하는 방향으로 마네킹이 회전한다.
    3. 카메라 지지대 길이 : 450cm
    4. 카메라 회전 : 마우스 상하좌우 이동에 따라 카메라 짖지대가 상하좌우로 회전
    5. 카메라 줌 : 카메라 시선과 캐릭터 사이에 장애물이 감지되면 캐릭터가 보이도록 카메라를 장애물 앞으로 줌인

    SpringArm 컴포넌트는 삼인칭 시점의 카메라 설정 구현시 편리한 컴포넌트이다.
    SpringArm 컴포넌트에도 마찬가지로 플레이어 컨트롤러의회전 값을 활용하도록 여러 속성들이 설계돼 있다.

    ABCharacter 클래스에 SetControlMode 멤버 함수를 작성하고 0번 모드에 GTP 방식의 삼인칭 조작 기능을 설정한다.
    생성자에 SetControlMode(0)을 선언해준다.
    
    void AABCharactter::SetControlMode(int32 ControlMode)
    {
        if (ControlMode == 0)
        {	
            // 스프링암의 길이
            SpringArm->TargetArmLength = 450.f;
            // 부모 액터와의 상대적인 각도 (TPS의 경우 마우스로 제어 받기에 0으로 초기화)
            SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
            // true면 대상(스프링암)이 입력에 따라 회전
            SpringArm->bUsePawnControlRotation = true;
            // 각각 pitch, yaw, roll 방향으로 회전이 가능한지 확인
            SpringArm->bInheritPitch = true;
            SpringArm->bInheritYaw = true;
            // 카메라와 플레이어 사이에 물체가 있을 때 카메라가 앞으로 오도록 설정
            SpringArm->bDoCollisionTest = true;
            // true면 카메라 시점에 따라 캐릭터 움직이는 방향(Yaw) 전환
            bUseControllerRotationYaw = false;
            //bOrientRotationToMovement true면 입력받는 방향으로 폰이 회전함(단 액터의 회전값을 받음)
            //bUseControllerDesiredRotation true면 입력받는 방향으로 폰이 회전함(컨트롤러의 회전값을 받음)
        }
    }

    이러면 캐릭터는 회전하지 않고 카메라만 움직에 따라 회전한다.

    이를 카메라의 방향을 중심으로 움직이도록 이동 방향을 변경해준다.
    이동을 위해 FVector 값이 필요하며 이 값은 컨트롤 회전, FRotator에 대한 데이터를 통해 얻는다.

    회전과 방향의 관계

    액터의 회전 값 (0, 0, 0)은 액터가 바라보는 방향이 월드의 X축 방향 (1, 0, 0)임을 의미한다.
    월드의 X축 방향은 기본 회전 값이 대응하는 방향 값이다.
    액터가 회전하면 액터의 시선 방향도 자연스럽게 다른 값으로 변한다.

    스프링암의 회전 값은 옵션에 의해 컨트롤 회전 값과 동일하므로 컨트롤 회전 값이
    카메라가 바라보는 방향이라고 할 수 있다.
    이 컨트롤 회전 값으로부터 회전 행렬을 생성한 후 원하는 방향 축을 대입해 캐릭터가 움직일 방향 값을 가져온다.
    UE에서 시선 방향은 X축, 우측 방향은 Y축을 의미한다.

    void AABCharactter::UpDown(const float& Value)
    {
        //AddMovementInput(GetActorForwardVector(), Value);
        AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), Value);
    }

    void AABCharactter::LeftRight(const float& Value)
    {
        //AddMovementInput(GetActorRightVector(), Value);
        AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), Value);
    }

    이러면 카메라의 시선 방향 기준으로 캐릭터가 이동한다.
    하지만 캐릭터에 회전 기능이 없기에 월드의 X축을 향해 바라보기만 한다.
    이는 캐릭터가 움직이는 방향으로 캐릭터를 자동으로 회전시켜주는 캐릭터 무브먼트 컴포넌트
    OrientRotationToMovement 기능을 사용한다.
    회전 속도를 함께 지정해 이동 방향으로 캐릭터가 부드럽게 회전하도록 한다.

    SetControl함수의 if 안에 
    
		//bOrientRotationToMovement true면 입력받는 방향으로 폰이 회전함(단 액터의 회전값을 받음)
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// 폰의 회전 속도 지정
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		// bUseControllerDesiredRotation true면 입력받는 방향으로 폰이 회전함(컨트롤러의 회전값을 받음)
		GetCharacterMovement()->bUseControllerDesiredRotation = false;

    코드를 추가해준다.
*/