/*
    애니메이션 몽타주

    캐릭터에 공격 기능을 넣기 위해 공격 입력을 설정하고
    공격 입력을 누를 때마다 연속된 모션으로 공격하도록 애니메이션 긴으을 구현한다.

    4개의 공격 애니메이션으로 연속 공격 명령을 내리면 콤보 공격처럼
    각 공격 애니메이션ㅇ르 순서대로 연계해야된다.

    스테이트 머신을 사용해 구현할 수 있지만 스테이트를 계쏙 추가해야돼
    설계가 복잡해진다는 단점이 있다.

    UE는 state machine의 확장 없이 특정 상황에서 원하는 애니메이션을 발동시키는
    애니메이션 몽타주라는 기능을 제공한다.

    애니메이션 창에서 Create Asset -> creat anim montage를 생성하면 편집화면으로 이동한다.

    몽타주는 섹션(Section)을 단위로 애니메이션을 관리한다.
    본래 몽타주는 촬영된 화면이나 인쇄된 종이를 떼어 붙여서 새로운 장면이나 이미지를 만드는 미술 기법이다.

    애니메이션 몽타주도 여러 애니메이션 클립들의 일부를 떼어내고 붙여서 새로운 애니메이션을 생성하는 기법이다.
    이때 섹션 단위로 애니메이션들을 자르고 붙이는 작업을 한다.

    공격 애니메이션들을 끌어다 놓고 우클릭으로 몽타주 섹션을 각각만들어준다.
    애니메이션의 Endtime을 수정해 공격 모션들이 이어지도록 한다.

    C++에서 애님 인스턴스에 멤버 함수와 변수를 생성하고 함수를 실행하면 몽타주 애니메이션 재생하는 기능을 만든다.

    몽타주 애셋과 과련된 명령은 항상 몽타주 애셋을 참조하기에 생성자에서 이를 미리 설정해두면 편리하다.

    Montage_IsPlaying함수를 사용해 현재 모아주가 재생하는지 파악하고, 아니라면
    Montage_Play 함수를 사용해 재생하도록 로직을 구현한다.

    AnimationInstance.h
    ...
    void PlayAttackMontage();
    ...
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;
    위 코드를 추가해준다.

    AnimationInstance.cpp
    생성자...
    static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Book/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
    ...
    void UABAnimInstance::PlayAttackMontage()
    {
        if (!Montage_IsPlaying(AttackMontage))
        {
            Montage_Play(AttackMontage, 1.f);
        }
    }

    컴파일 이후 블루프린트를 열어보면 해당 속성에 몽타주 애셋이 자동할당된다.

    몽타주에게 재생 명령을 내려도 애니메이션 블루프린트에서 이를 재생하려면 모타주 재생 노드를
    애님 그래프에 추가해야 한다.
    이 몽타주 재생 노드를 적당한 애니메이션 재생 흐름 사이에 끼워 넣으면 해당 타이밍에 발동한다.

    모든 상황에 몽타주를 재생할 예정이므로 애님 그래프의 최종 애니메이션 포즈와 스테이트 머신 사이에 추가한다.

    애님 그래프를 우클릭애 노드 목록에서 DefaultSlot 노드를 생성하고 몽타주 애니메이션을 넣을 곳인
    최종 애니메이션 포즈와 스테이트 머신 사이에 추가해준다.

    Character.cpp
    ...
    void AABCharactter::Attack()
    {
        auto AnimInstance = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
        if (nullptr == AnimInstance) return;

        AnimInstance->PlayAttackMontage();
    }
    ..
    
    이 코드를 적용해 공격 애니메이션을 재생하게 한다.

*/