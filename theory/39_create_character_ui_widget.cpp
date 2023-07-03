/*
    캐릭터 위젯 UI 제작

    캐릭터의 HP 값이 시각적으로 보이도록 UI 위젯을 제작하고 캐릭터에 부착한다.

    우클릭 -> User Interface -> Widget Blueprint -> User 로 위젯 애셋을 생성한다.

    위젯 애셋을 더블 클릭해 제작하는 창을 보면 왼쪽에 Pallete와 Hierachy 등을 볼 수 있다.

    지금은 캐릭터 위에 작게 프로그래스바로만 표시할 예정이라 Pallete에서 Progressbar를 
    제작창에 끌어와 생성한 후 우측 상단에 사이즈를 fillscreen에서 custum으로 변경해준다.
    그리고 150x50으로 크기를 변경해준다.

    그리고 왼쪽에서 이 바의 이름을 변경해준다.
    프로그래믓 바를 우클릭한 후 wrap -> vertical box를 해주면 세로로 UI 컨트롤을 정렬해주는
    Verticla Box가 ProgressBar를 감싸준다.

    그 후 Pallete에서 Spacer를 검색해 progressbar 위아래로 넣어준다.
    그럼 Vertical Box는 3개의 컨트롤을 가지는데, 레이아웃을 잡기위해 Box 영역 내에서 이들이
    나눠가질 영역을 지정해준다.

    각 컨트롤의 Details 창에서 Size를 auto -> fill로 바꾼후 40 : 20 : 40으로 변경해준다.

    이제 프로그래스바의 Fill Color를 빨간색으로 설정해준뒤 Percent 속성을 드래그해
    변화를 보면 왼쪽부터 채워지는 걸 알 수있따. (Fill type으로 채워지는 방향 변경가능)

    UE은 액터에 UI 위젝을 부착할 수 있도록 UWidgetComponent라는 클래스를 제공한다.
    Character.h
    ...
	// UI 위젯
	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* HPBarWidget;
    ...

    만약 컴파일 중 '확인할 수 없는 외부 참조(Unable to find ...)' 에러가 발생하면 현 프로젝트에
    UI 관련 엔진 모듈이 지정되 있지 않아 그런 것이므로 모듈을 설정해줘야 한다. (40_setting_module...)

    생성된 위젯 컴포넌트는 캐릭터 머리 위로 오도록 위치를 조정하고, 제작한 위젯 블루프린트 애셋의
    reference를 사용해 애셋의 클래스 정보를 위젯 컴포넌트의 WidgetClass로 등록한다.

    그리고 UI 위젯은 항상 플레이어를 향해 보도록 Screen 모드로 지정하고, 크기는 작업 공간에서
    설정한 것과 동일한 (150, 50)으로 지정한다.
    
    Character.cpp
    #include "Components/WidgetComponent.h"
    ... 생성자
    HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
    HPBarWidget->SetupAttachment(GetMesh());
    // 캐릭터 머리 위로 오도록 위치를 조정
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	// 항상 플레이어를 향해 보도록 Screen 모드로 지정한다.
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		// 생성한 위젯과 크기가 맞게 150x50으로 크기 조정
		HPBarWidget->SetDrawSize(FVector2D(150.f, 50.f));
	}
    ...




*/