// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameInstance.h"

UABGameInstance::UABGameInstance()
{
	// 데이터 애셋 가져오기
	FString CharacterDataPath = TEXT("/Game/Book/GameData/ABCharacterData.ABCharacterData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_ABCHARACTER(*CharacterDataPath);
	ABCHECK(DT_ABCHARACTER.Succeeded());
	ABCharacterTable = DT_ABCHARACTER.Object;
	ABCHECK(ABCharacterTable->GetRowMap().Num() > 0);

}

void UABGameInstance::Init()
{
	Super::Init();
	// 데이터가 잘 로딩됐는지 확인용으로 20레벨의 데이터 출력 (확인용)
	//ABLOG(Warning, TEXT("DropExp of Level 20 ABCharacter : %d"), GetABCHaracterData(20)->DropExp);
}

FABCharacterData* UABGameInstance::GetABCHaracterData(int32 Level)
{
	// 원하는 레벨의 데이터 로딩
	return ABCharacterTable->FindRow<FABCharacterData>(*FString::FromInt(Level), TEXT(""));
}
