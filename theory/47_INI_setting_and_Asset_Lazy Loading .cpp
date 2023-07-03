/*
    INI 설정과 애셋의 지연 로딩

    새로운 모듈에 추가한 ABCharacterSetting은 앞으로 사용할 캐릭터 애셋의 목록을 보관한다.
    UE은 언리얼 오브젝트의 기본값을 유연하게 관리하도록 외부 INI 파일에서 기본 속성 값을 지정하는 기능을 제공한다.

    애셋은 경로 정보만 알면 프로그램에서 이를 참조해 로딩할 수 있다.
    이 애셋 경로 정보를 보관하기 위해 UE은 FSoftObjectPath라는 클래스를 제공한다.

    언리얼 오브젝트가 기본값을 INI 파일에서 불러들이려면 UCLASS 매크로에 config 키워드를
    추가해 불러들일 INI 파일의 이름을 지정하고, 불러들일 PROPERTY 속성에는 config 키워드를 선언해야한다.
    이러면 UE은 언리얼 오브젝트르 초기화할 때 해당 속성의 값을 INI 파일에서 읽어 설정한다.
    ABCharacterSetting.h
    // 언리얼 오브젝트의 기본값을 INI 파일에서 불러들이기 위해 config 키워드 추가
    UCLASS(config=ArenaBattle)
    class ARENABATTLESETTING_API UABCharacterSetting : public UObject
    {
        GENERATED_BODY()
        
    public:
        UABCharacterSetting();
        
        // 불러들일 PROPERTY 속성에 config 키워드 선언해 INI 파일에서 해당 속성 값을 읽어 설정하게 한다.
        UPROPERTY(config)
        TArray<FSoftObjectPath> CharacterAssets;
    };
    ABCharacterSetting.cpp
    UABCharacterSetting::UABCharacterSetting() {}

    UCLASS 매크로 내 config 키워드에 있는 ArenaBattle이라는 설정으로 인해, UE은 초기화 단계에서
    config 폴더에 위치한 DefaultArenaBattle.ini 파일을 읽어들여 CharacterAssets 값을 설정한다.
    즉, config 폴더에 DefaultArenaBattle.ini 파일을 생성해준다.
    이 파일에는 생성한 변수의 이름으로 애셋들의 경로를 설정해준다.
    ex)
        [/Script/ArenaBattleSetting.ABCharacterSetting]
        +CharacterAssets=/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Barbarous.SK_CharM_Barbarous 

    UE가 초기화되면 엔진 구동에 필요한 모듈이 순차적으로 로딩된다.
    모듈이 로딩되며 자신에게 속한 모든 언리얼 오브젝트의 기본값을 지정해 생성해내는데,
    이를 클래스 기본 객체(Class Default Object)라고 한다.
    그래서 엔진이 초기화되면 도느 언리얼 오브젝트 클래스 기본 객체가 메모리에 올라간 상태가 된다.

    메모리에 올라간 클래스 기본 객체는 GetDefault 함수를 통해 엔진이 종료할 때 까지 가져올 수 있다.

    ABCharacterSetting 언리얼 오브젝트의 클래스 기본 객체는 엔진 초기화 단계에서 생성자를 거쳐 
    INI에서 설정한 값이 할당되므로, ArenaBattleSetting 모듈 이후에 로딩되는
    ArenaBattle모듈에서 GetDefault 함수를 사용하면 INI에서 지정한 애셋의 목록 정보를 가져올 수 있다.

    ArenaBattle 모듈의 ABCharacter에서 ArenaBattleSetting 모듈의 ABCharacterSetting에서
    캐릭터 애셋 목록을 얻어오려면 ArenaSetting 모듈의 빌드 규칙을 지정하는
    ArenaBattle.Build.cs 파일에서 ArenaBattleSEtting 모듈을 사용하도록 참조할 모듈 목록을 추가한다.
    구현부가 모여있는 Private 폴더에서만 setting 모듈을 사용할 예정이라 Private module에 추가한다.

    새로운 모듈을 추가했다면 GetDefault 함수를 사용해 ABCHaracter에서 애셋 목록을 읽어본다.
    ABCharacter.cpp
    ... 생성자
    // ArenaBattleSetting으로 추가한 새 모듈에서 애셋 목록을 읽어들인다.
	auto DefaultSetting = GetDefault<UABCharacterSetting>();
	if (DefaultSetting->CharacterAssets.Num() > 0)
	{
		for (auto CharacterAsset : DefaultSetting->CharacterAssets)
		{
			ABLOG(Warning, TEXT("CHaracterAsset : %s"), *CharacterAsset.ToString());
		}
	}

    성공적으로 잘 가져오면 이 코드를 제거하고 NPC가 생성될 때 랜덤하게
    목록 중 하나를 골라 캐릭터 애셋을 로딩하도록 기능을 변경해본다.
    UE은 게임 진행 중에도 비동기 방식으로 애셋을 로딩하도록 FStreamableManage라는 클래스를 제공한다.
    이 매니저 클래스는 프로젝트에서 하나만 활성화하는 것이 좋기에 현재 프로젝트에서
    유일한 인스턴스로 동작하는 ABGameInstance 클래스에서 이를 멤버 변수로 선언한다.

    FStreamableManager에서 비동기 방식으로 애셋을 로딩하는 명령은 AsyncLoad다.
    해당 함수에 FStreamableDelegate 형식의 Delegate를 넘겨줄 경우, 애셋이 로딩을
    완료하면 해당 델리게이트에 연결된 함수를 호출해준다.

    FStreamableDelegate 형식으로 멤버를 선언하는 것보단 Delegate에서 제공하는
    CreateUObject 명령을 사용해 즉석에서 Delegate를 생성함으로 함수와 연동시킨 후
    넘겨주는 방식이 간편하다.
    ABGameInstance.h
    #include "Engine/StreamableManager.h"
    ...
    FStreamableManager StreamableManager;
    ...
    ABCharacter.h
    ...
    // INI 파일에서 애셋 경로를 읽어서 캐릭터 애셋 가져오기
	void OnAssetLoadCompleted();
	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;
    ...
    ABCharacter.cpp
    #include "ABGameInstance.h"
    ..
    void AABCharacter::BeginPlay()
    {
        ...
        // AI가 컨트롤 한다면
        if (!IsPlayerControlled())
        {
            // ArenaBattleSetting에서 INI 파일의 애셋 경로를 읽어온다.
            auto DefaultSetting = GetDefault<UABCharacterSetting>();
            // 경로들 중 하나 랜덤하게 가져오기
            int32 RandIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
            CharacterAssetToLoad = DefaultSetting->CharacterAssets[RandIndex];

            auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
            if (nullptr != ABGameInstance)
            {   
                // AsyncLoad를 통해 비동기 방식으로 애셋을 로딩
                // 애셋이 로딩을 완료하면 해당 Delegate에 연결된 함수를 호출해준다.
                // Delegate에서 제공하는 CreateUObject 명령을 사용해 즉석에서 Delegate를 생성해 함수와 연동시켜준다.
                AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, 
                    FStreamableDelegate::CreateUObject(this, &AABCharactter::OnAssetLoadCompleted));
            }
        }
        ...
    }
    ...
    void AABCharactter::OnAssetLoadCompleted()
    {
        // 스켈레톤 메시에 불러온 애셋 선언
        USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
        // 핸들러 초기화
        AssetStreamingHandle.Reset();
        if (nullptr != AssetLoaded)
        {
            // 메시 세팅
            GetMesh()->SetSkeletalMesh(AssetLoaded);
        }
    }
    ...

    이러면 NPC에 한해 캐릭터 메시가 랜덤으로 로딩된다.

    GameInstacne는 게임 내 유일한 인스턴스를 가지는 Singleton처럼 동작하기에 이번엔 이를 사용했다.
    언리얼에서는 이와 별도로 싱글톤으로 동작하는 언리얼 오브젝트를 지정할 수 있다.
    프로젝트 설정의 일반 세팅에 있는 Default Classes 섹션의 고급 섹션에서 이를 지정할 수 있다.

    StreamableManage는 엔진 모듈 내에 존재하는 UAssetManager라는 오브젝트에 이미 선언돼 있다.
    Engine/AssetManager.h 헤더 파일을 포함한 뒤 UAssetManager::GetStremalbeManaer() 함수로 대신할 수 있따.
*/