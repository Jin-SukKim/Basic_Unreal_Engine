/*
    애니메이션의 설정

    현재 폰은 팔 벌린 T 형태로 정지해있는데 이를 보안하기 위해 스켈레탈 메시에
    애니메이션을 설정한다.

    애니메이션 파일을 언리얼 프로젝트에 사용하기 위해 임포트를 해준다.

    그후 애니메이션 애셋의 래퍼런스를 복사해 기본 애니메이션 애셋으로 지정해준다.

    게임의 실행 중에 애셋을 로드하는 명령어는 LoadObject<타입>이며, 
    이를 사용해 애니메이션 애셋을 불러들인다.
    LoadObject<UAnimationAsset>(nullptr, TEXT(reference))

    ex)
        Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
        UAnimationAsset* AnimAsset = LoadObject<UAnimationAsset>(nullptr, TEXT("/Game/Book/Animations/WarriorRun.WarriorRun"));
        if (AnimAsset != nullptr)
        {
            Mesh->PlayAnimation(AnimAsset, true);
        }

    이렇게 코드로 애니메이션을 지정할 수 있지만 게임의 규모가 커지면 이런 방식의 애니메이션 재생은
    관리적인 한계에 부딪힌다.
    그래서 UE는 체계적으로 애니메이션 시스템을 설계하도록 별도의 애니메이션 시스템을 제공하는데,
    이를 애니메이션 블루프린트라고 한다.

    애니메이션 블루프린트 시스템은 프로그래밍이 아닌 시각적 도구로 개발하도록 설계돼 있는 것이 특징이다.

    콘텐츠 브라우저에서 애니메이션 블루프린트 애셋을 생성해준다.
    애니메이션 블루프린트에는 여러 기능이 있는데 애니메이션을 설계하는 애님 그래프(Anim Graph)라는
    작업 환경을 선택한다.

    그 후 애니메이션 블루프린트 우측 하단에 있는 애셋 브라우저 윈도우를 선택하고,
    목록에 위치한 WarriorRun을 애님 그래프에 끌어다 놓는다.

    그리고 WarriorRun 애니메이션 재생 노드에서 사람 모양의 핀을 최종 애니메이션 노드에 드래그해 연결한다.
    컴파일과 저장을 누르면, 프리뷰 인스턴스 디버깅에 의해 왼쪽 프리뷰에서 해당 애니메이션이 재생된다.

    애니메이션 블루프린트는 애님 그래프 로직에 따라 동작하는 캐릭터 애니메이션 시스템을 구동시키는데,
    이러한 애니메이션 시스템은 C++ 프로그래밍의 애님 인스턴스(Anim Instance)라는 클래스로 관리된다.

    스켈레탈 메시 컴포넌트는 자신이 관리하는 캐릭터의 애니메이션을 이 애님 인스턴스에 위임하는 구조로 설계돼 있다.
    이 애니메이션 블루프린트를 실행시키려면 블루프린트 애셋의 클래스 정보를 애님 인스턴스 속성에 지정해줘야 한다.

    스켈레탈 메시 컴포넌트에 애니메이션 블루프린트의 클래스 정보를 등록하면,
    컴포넌트는 인스턴스를 생성해 애니메이션을 관리하도록 동작한다.

    애니메이션 블루프린트를 사용해 애니메이션을 재생하는 코드

    Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    // 클래스 정보 이므로 _C를 붙여준다.
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		Mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

    이 ConstructorHelper는 Consturctor에서 사용해야한다.
    생성자가 아닌 다른곳에서 사용한다면 crash로 인해 게임이 실행되지 않는다.
*/