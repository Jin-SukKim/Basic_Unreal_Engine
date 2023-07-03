// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;	// tick �Լ��� ������� �ʾƼ� false�� ����
	// InitializeComponent �Լ��� ����ϱ� ���� �� ���� true�� �������ش�.
	bWantsInitializeComponent = true;

	Level = 1;
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// ���� ���۽� �ʱ�ȭ �Լ�
void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// ó�� ���� ������ ����
	SetNewLevel(Level);
}

void UABCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	// �����͸� �������� ���� Game Instance ĳ��Ʈ
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	// ABGameInstance�� �´��� Ȯ��
	ABCHECK(nullptr != ABGameInstance);
	// GetABCharacterData �Լ��� ����� CSV ������ ��������
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
	// ��������� HP ���
	CurrentHP = FMath::Clamp<float>(CurrentHP - NewDamage, 0.f, CurrentStatData->MaxHP);
	if (CurrentHP <= 0.f)
	{	
		// HP�� 0 ���ϸ� ������ �˷��ش�.
		OnHPIsZero.Broadcast();
	}
	*/

	// UI ������ ������ ���� ���ο� ����� ��� �Լ�
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
	// float�� ���� 0�� �񱳽� �̼��� ���� ���� ���� �ִ��� ���� �Ǵ��ϴ� ���� ����.
	// UE�� ���� ������ ������ ������ �� ����ϵ��� KINDA_SMALL_NUMBER ��ũ�θ� �����Ѵ�.
	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		// HP�� 0 ���� ��ó�� ������ �˷��ش�.
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

