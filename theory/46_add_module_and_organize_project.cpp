/*
    프로젝트의 정리와 모듈의 추가

    언리얼 소스 코드 구조를 살펴보면 언리얼 오브젝트에 관련된 헤더들은 Classes 폴더를 사용하고,
    외부에 공개하는 선언 파일은 Public 폴더에서, 외부에 공개하지 않는 정의 파일은 Private 폴더에서 보관하고 있다.
    이렇게 프로젝트의 폴더 구조를 변경한다.

    윈도우 탐색기를 열고 프로젝트의 Source/ArenaBattle 폴더에서 Public, Private 폴더를 만들어준다.'

    확장자가 .h인 ㅎ헤더 파일들은 모두 public 폴더로 .cpp 파일들은 Private 폴더로 옮긴다.
    그럼 현재 폴더엔 Build.cs 파일만 남는다.

    프로젝트 폴더로 이동해 uproject 파일 우클릭 -> Generate Visuale Studio project files 선택
    이러면 솔루션 탐색기의 폴더 구조를 보면 프로젝트 구성이 바뀐다.
    컴파일하고 에디터를 실행하면 UE의 콘텐츠 브라우저의 C++ 클래스 폴더 구성도 함께 변경된다.

    UE은 주 게임 모듈(Primary Game Module)을 사용해 게임 프로젝트의 로직을 관리한다.
    여태 작업한 코드는 ArenaBattle(프로젝트명)이란 주 게임 모듈에서 관리하고 있었다.
    그런데 주 게임 모듈 외에 다른 모듈을 게임 프로젝트에 추가하고 로직을 분리해 관리할 수도 있다.

    언리얼 에디터는 C++ 프로젝트 생성 시 Primary Game Modlue을 자동으로 생성해주지만,
    추가 모듈 생성 기능은 제공하지 않는다.

    새로운 모듈을 추가하려면 언리얼 빌드 규칙을 이해하고 이에 따라 폴더와 파일을 생성해야 한다.
    추가 모듈 제작을 위해 필요한 요소

    1. 모듈 폴더와 빌드 설정 파일 : 모듈 폴더와 모듈명으로 된 Build.cs 파일
    2. 모듈의 정의 파일 : 모듈명으로 된 .cpp 파일

    추가로 정의 파일이 참조할 수 있게 모듈명으로 된 헤더 파일도 넣어준다.
    Source/ArenaBattleSetting 폴더에 새로운 모듈을 위한 필수 파일을 생성해준다.

    추가로 모듈을 제작하면 uproject에서 vsproject를 재생성해주면 새로운 모듈이 담긴 폴더가
    프로젝트에 추가된다.

    이제 VS에서 추가한 모듈을 빌드하도록 ArenaBattle.Target.cs 파일과 
    ArenaBattleEditor.Target.cs 파일 정보를 수정해준다
    각 Target.cs 파일은 Game build와 Editor build 설정을 지정해준다.

    ExtraModuleNames.AddRange( new string[] { "ArenaBattle", "새로운 모듈 이름" } );
    이 라인에 모듈을 추가해주면 빌드 명령 시 새로운 모듈을 컴파일한다.
    Build가 완료되면 Binaries 폴더에 새로운 모듈 이름의 파일이 생성되는지 확인한다.
    
    새 DLL 파일이 생성돼면 언리얼 에디터가 이 DLL 파일을 로딩하도록 수정해준다.
    이를 위해 uproject 파일에 새로운 모듈에 대한 정보를 기입해준다.
    
    모듈 정보 기입 시 새로운 ArenaBattlesetting 모듈을 다른 모듈보다 먼저 로딩하도록
    LoadingPhase 값을 "PreDefault"로 설정하고 ArenaBattle 모듈이 ArenaBattleSetting 모듈에 대해
    의존성을 가지도록 설정한다.

    ArenaBattle.uproject에 
    ...
    "Modules": [
		{
			"Name": "ArenaBattleSetting",
			"Type": "Runtime",
			"LoadingPhase": "PreDefault",
			"AdditionalDependencies": [
				"CoreUObject"
			]
		},
		{
			"Name": "ArenaBattle",
			"Type": "Runtime",
			"LoadingPhase": "Default",
			"AdditionalDependencies": [
				"Engine",
				"UMG",
				"AIModule",
				"ArenaBattleSetting"
			]
		}
        ...
    을 ArenaBattle 모듈 위에 선언해준다.

    그런데 에디터에 로딩하고 언리얼 에디터의 콘텐츠 브라우저의 C++ 클래스 설정을 살펴보면 모듈 목록에
    ArenaBattle 모듈만 나타나는데 새로운 모듈에 속한 언리얼 오브젝트가 하나도 없기 때문이다.

    ArenaBattleSetting 모듈 설정에 언리얼 오브젝트를 추가한다.
    Object를 부모로 하는 ABCharacterSetting 클래스를 생성해준다.
    클래스 생성시 Name 작성하는 박스 옆에 보면 ArenaBattle 모듈로 설정되있는데 ArenaBattleSetting 모듈로 변경한 뒤
    클래스 생성해주면 된다.

    컴파일을 진행해주고 에디터를 보면 ArenaBattleSetting 폴더가 생성되고 ABCharacterSetting 오브젝트가 들어가있는 것을 확인할 수 있다.
*/