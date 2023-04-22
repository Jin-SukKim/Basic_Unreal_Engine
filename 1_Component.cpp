/*
    컴포넌트(Component)는 액터의 역할에 따라 각 기능의 사용 여부를 정해준다.

    1. 스태틱메시 컴포넌트(staticmesh compoenent) :
        애니메이션이 없는 모델링 애셋인 스태틱메시를 사용해 시각적인 기능과
        물리적인 기능을 제공하는 모듈 (주로 배경 물체에 사용)

    2. 스켈레탈메시 컴포넌트(SkeletalMesh Component) :
        애니메이션 정보가 있는 모델링 애셋인 스켈레탈 메시를 사용해 시각적인 기능과
        애니메이션, 그리고 캐릭터의 물리 기능을 제공하는 모듈 (주로 캐릭터에 사용)

    3. 콜리전 컴포넌트(Collision Component) :
        구/박스/캡슐로 지정한 여역에 물리적인 기능을 설정하기 위해 제공하는 모듈
        (시각적인 기능은 없다)
    
    4. 카메라 컴포넌트(Camera Component) :
        가상 세계에서 보여지는 혅 ㅐ상황을 플레이어의 모니터 화면에 출력해주는 기능

    5. 오디오 컴포넌트(Audio Compnent) :
        가상 세계에서 소리를 발생시키는 데 사용하는 기능

    6. 파티클 시스템 컴포넌트(Particle System Component) :
        파티클시스템으로 설계된 이펙트를 화면에 보여주기 위한 기능

    7. 라이트 컴포넌트(Light Component) :
        전구, 헤드라이트 등과 같이 물체에 광원 효과를 부여하는 기능

    8. 무브먼트 컴포넌트(Movement Component) :
        물체에 특정한 움직임을 부여하는 기능
*/