// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;	// tick 함수를 사용하지 않아서 false로 설정
	// InitializeComponent 함수를 사용하기 위해 이 값을 true로 설정해준다.
	bWantsInitializeComponent = true;

	Level = 1;
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// 게임 시작시 초기화 함수
void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// 처음 스탯 데이터 설정
	SetNewLevel(Level);
}

void UABCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	// 데이터를 가져오기 위해 Game Instance 캐스트
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	// ABGameInstance가 맞는지 확인
	ABCHECK(nullptr != ABGameInstance);
	// GetABCharacterData 함수를 사용해 CSV 데이터 가져오기
	CurrentStatData = ABGameInstance->GetABCHaracterData(NewLevel);
	if (nullptr != CurrentStatData)
	{
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHP);
		CurrentHP = CurrentStatData->MaxHP;
	}
	else
	{
		ABLOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
	}
}

void UABCharacterStatComponent::SetDamage(float NewDamage)
{
	ABCHECK(nullptr != CurrentStatData);
	/*
	// 대미지입은 HP 계산
	CurrentHP = FMath::Clamp<float>(CurrentHP - NewDamage, 0.f, CurrentStatData->MaxHP);
	if (CurrentHP <= 0.f)
	{	
		// HP가 0 이하면 죽음을 알려준다.
		OnHPIsZero.Broadcast();
	}
	*/

	// UI 위젯과 연동을 위한 새로운 대미지 계산 함수
	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.f, CurrentStatData->MaxHP));
}

float UABCharacterStatComponent::GetAttack()
{
	ABCHECK(nullptr != CurrentStatData, 0.f);
	return CurrentStatData->Attack;
}

void UABCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();
	// float의 값을 0과 비교시 미세한 오차 범위 내에 있는지 보고 판단하는 것이 좋다.
	// UE는 무시 가능한 오차를 측정할 때 사용하도록 KINDA_SMALL_NUMBER 매크로를 제공한다.
	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		// HP가 0 이하 근처면 죽음을 알려준다.
		CurrentHP = 0.f;
		OnHPIsZero.Broadcast();
	}
}

float UABCharacterStatComponent::GetHPRatio()
{
	ABCHECK(nullptr != CurrentStatData, 0.f);
	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.f : (CurrentHP / CurrentStatData->MaxHP);
}

int32 UABCharacterStatComponent::GetDropExp() const
{
	return CurrentStatData->DropExp;
}

/*
// Called every frame
void UABCharacterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
*/

