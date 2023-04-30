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

    

*/