// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameInstance.h"

UABGameInstance::UABGameInstance()
{
	// ������ �ּ� ��������
	FString CharacterDataPath = TEXT("/Game/Book/GameData/ABCharacterData.ABCharacterData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_ABCHARACTER(*CharacterDataPath);
	ABCHECK(DT_ABCHARACTER.Succeeded());
	ABCharacterTable = DT_ABCHARACTER.Object;
	ABCHECK(ABCharacterTable->GetRowMap().Num() > 0);

}

void UABGameInstance::Init()
{
	Super::Init();
	// �����Ͱ� �� �ε��ƴ��� Ȯ�ο����� 20������ ������ ��� (Ȯ�ο�)
	//ABLOG(Warning, TEXT("DropExp of Level 20 ABCharacter : %d"), GetABCHaracterData(20)->DropExp);
}

FABCharacterData* UABGameInstance::GetABCHaracterData(int32 Level)
{
	// ���ϴ� ������ ������ �ε�
	return ABCharacterTable->FindRow<FABCharacterData>(*FString::FromInt(Level), TEXT(""));
}
