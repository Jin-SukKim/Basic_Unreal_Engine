/*
    외부 데이터 활용

    엑셀 데이터로부터 캐릭터의 능력치를 체계적으로 관리할 수 있도록 테이블 데이터를 UE에 불러온다.
    또한 게임 데이터를 효과적으로 관리하기에 적합한 게임 인스턴스 클래스와
    방대한 로직을 분산시킬 수 있도록 액터 컴포넌트를 설계해 액터에 적용하는 방법을 알아본다 .

    액셀에 저장돼 있는 캐릭터의 스탯 데이터 테이블을 UE에 불러오는 기능을 만든다.

    캐릭터 스탯 데이터는 게임의 기반을 이루는 변하지 않는 데이터이므로 보통 게임 앱이 초기화될 떄 불러들인다.
    UE은 게임 앱을 관리하기 윟나 용도로 게임 인스턴스라는 언리얼 오브젝트를 제공한다.
    
    게임 인스턴스가 캐릭터의 스탯을 관리하도록 설계하면 게임 앱 초기화시 스탯 데이터를 불러들이고,
    종료 시딱지 스탯 데이터는 보존된다.

    GameInstance를 부모 클래스로 하는 새로운 클래스를 생성하고 캐릭터 스탯을 관리하는 기능을 만든다.
    새롭게 생성된 게임 인스턴스가 게임 앱을 관리하기 위해서는 Project setting의 Map & Mode에 있는
    GameInstance 항목을 변경시켜준다.

    게임 앱 초기화시 UE는 GameInstance의 Init 함수를 호출한다.

    게임을 시작하는 과정

    게임 앱의 초기화 -> 레벨에 속한 액터의 초기화 -> 플레이어의 로그인 -> 게임의 시작
    UGameInstance::Init -> AActor::PostInitializeComponents -> AGameMode::PostLogin -> AGameMode::StartPlay, AActor::BeginPlay

    게임앱을 초기화하고 월드를 생성한 후 월드에 레벨을 로딩해 스테이지를 생성하고 나면,
    플레이어가 로그인해 준비를 마쳐야 게임이 시작된다.

    게임 데이터를 관리할 게임 인스턴스를 설정하고 나면 게임 데이터를 프로젝트에
    import하고 불러들이는 기능을 구현한다.

    UE는 행과 열로 구성된 테이블 데이터를 불러오는 기능을 제공한다.
    엑센 파일 형식 자체를 사용할 수는 없고 CSV 파일 형식으로 변환해야 한다.
    CSV파일로 저장하면 쉼표로 모든 데이터가 분리돼 있다.

    UE에서 불러들이기 윟 ㅐ테이블 데이터의 각 열의 이름과 유형이 동일한 구조체를 선언해야 한다.
    UE에서 제공하는 FTableRowBase 구조체를 상속받은 FABCharaterData라는 이름의 
    구조체를 게임 인스턴스의 헤더에 선언한다.

    구조체 생성 시 UE가 지정한 규칙에 따라야 에디터 인터페이스에서 연동해 사용할 수 있다.
    언리얼 오브젝트 선언과 유사하게 USTRUCT 매크로를 구조체 선언 위에 넣고
    구조체 내부에는 GENERATED_BODY() 매크로를 선언한다.

    구조체의 골격을 완성하면 CSV 파일의 각 열의 이름과 동일한 멤버 변수를 타입에 맞춰 선언한다.
    이떄 테이블의 첫 번째에 위치한 Name 열 데이터는 UE에서 자동으로 키 값으로 사용하기에 제외한다.

    ABGameInstance.h
    ...
    #include "Engine/DataTable.h"
    ...
    // 데이터를 불러들이기 위한 구조체 선언
    USTRUCT(BlueprintType)
    struct FABCharacterData : public FTableRowBase
    {
        GENERATED_BODY() // 매크로

    public:
        FABCharacterData() : Level(1), MaxHP(100.f), Attack(10.f), DropExp(10), NextExp(30) {}

        // 외부 데이터 파일의 각 열의 이름과 동일한 변수를 타입에 맞춰 선언해준다.
        // 첫 번째 열 데이터는 UE에서 자동으로 키 값으로 사용돼 제외된다.
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
            int32 Level;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
            float MaxHP;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
            float Attack;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
            int32 DropExp;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
            int32 NextExp;
    };
    ...

    그러면 데이터 파일을 임포트할 수 있게된다.
    성공적으로 구조체를 선언했다면 DataTable Option에서 선언된 구조체가 보인다.

    게임 데이터 애셋을 성공적으로 임포트했다면 게임 인스턴스에서 이를 로딩한다.

    UE은 테이블 데이터를 관리하도록 DataTable이라는 언리얼 오브젝트를 제공한다.
    DataTable을 Game Instance의 멤버 변수로 선언하고 데이터 애셋의 래퍼런스를 복사한 후,
    데이터를 불러들인다.

    ABGameInstance.h
    ...
    // 레벨에 따른 데이터 얻기
	FABCharacterData* GetABCHaracterData(int32 Level);
    private:
        // 테이블 데이터를 관리하는 언리얼 오브젝트
        UPROPERTY()
        class UDataTable* ABCharacterTable;
    ...
    ABGameInstance.cpp
    ... 생성자
    // 데이터 애셋 가져오기
	FString CharacterDataPath = TEXT("/Game/Book/GameData/ABCharacterData.ABCharacterData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_ABCHARACTER(*CharacterDataPath);
	ABCHECK(DT_ABCHARACTER.Succeeded());
	ABCharacterTable = DT_ABCHARACTER.Object;
	ABCHECK(ABCharacterTable->GetRowMap().Num() > 0);
    ...
    void UABGameInstance::Init()
    {
        Super::Init();
        // 데이터가 잘 로딩됐는지 확인용으로 20레벨의 데이터 출력
        ABLOG(Warning, TEXT("DropExp of Level 20 ABCharacter : %d"), GetABCHaracterData(20)->DropExp);
    }

    FABCharacterData* UABGameInstance::GetABCHaracterData(int32 Level)
    {
        // 원하는 레벨의 데이터 로딩
        return ABCharacterTable->FindRow<FABCharacterData>(*FString::FromInt(Level), TEXT(""));
    }

*/