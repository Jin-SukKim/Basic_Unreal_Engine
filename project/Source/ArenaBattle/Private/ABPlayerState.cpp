// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABSaveGame.h"

AABPlayerState::AABPlayerState()
{
	CharacterLevel = 1;
	GameScore = 0;
	Exp = 0;
	GameHighScore = 0;
	CharacterIndex = 0;
	SaveSlotName = TEXT("Player1");
}

int32 AABPlayerState::GetGameScore() const
{
	ABLOG(Warning, TEXT("GetGameScore Score : %d"), GameScore);
	return GameScore;
}

int32 AABPlayerState::GetCharacterLevel() const
{
	return CharacterLevel;
}

int32 AABPlayerState::GetCharacterIndex() const
{
	return CharacterIndex;
}

void AABPlayerState::InitPlayerData()
{
	// 맨 처음 세이브 데이터 파일이 없으면 기본 세이브 파일 생성
	auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (nullptr == ABSaveGame)
	{
		ABSaveGame = GetMutableDefault<UABSaveGame>();
	}
	SetPlayerName(ABSaveGame->PlayerName);
	SetCharacterLevel(ABSaveGame->Level);
	GameScore = 0;
	GameHighScore = ABSaveGame->HighScore;
	Exp = ABSaveGame->Exp;
	CharacterIndex = ABSaveGame->CharacterIndex;
	// 최초에 플레이어 데이터 생성하자마자 바로 저장
	SavePlayerData();
}

float AABPlayerState::GetExpRatio() const
{
	// 얻을 수 있는 Exp가 0에 가까운 경우
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
		return 0.f;

	// UI의 ProgressBar에 표시되므로 Ratio 계산
	float Result = (float)Exp / (float)CurrentStatData->NextExp;
	ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
	return Result;
}

bool AABPlayerState::AddExp(int32 IncomeExp)
{
	if (CurrentStatData->NextExp == -1)
		return false;

	bool DidLevelUp = false;
	// 현재 Exp에 얻은 Exp 더해주기
	Exp = Exp + IncomeExp;
	// 충분한 경험치를 얻었다면 레벨업
	if (Exp >= CurrentStatData->NextExp)
	{
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(CharacterLevel + 1);
		DidLevelUp = true;
	}

	OnPlayerStateChanged.Broadcast();
	// 경험치 획득할 때마다 데이터 저장
	SavePlayerData();
	return DidLevelUp;
}

void AABPlayerState::AddGameScore()
{
	GameScore++;
	// 최고점 갱신
	if (GameScore >= GameHighScore)
	{
		GameHighScore = GameScore;
	}
	OnPlayerStateChanged.Broadcast();
	// 점수 획득할 때마다 데이터 저장
	SavePlayerData();
}

int32 AABPlayerState::GetGameHighScore() const
{
	return GameHighScore;
}

void AABPlayerState::SavePlayerData()
{
	// 새로운 세이브 데이터 언리얼 오브젝트
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
	NewPlayerData->PlayerName = GetPlayerName();
	NewPlayerData->Level = CharacterLevel;
	NewPlayerData->Exp = Exp;
	NewPlayerData->HighScore = GameHighScore;

	// Slot Name을 기준으로 slot에 데이터 저장하기
	if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0))
	{
		ABLOG(Error, TEXT("SaveGame Error!"));
	}
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);

	CurrentStatData = ABGameInstance->GetABCHaracterData(NewCharacterLevel);
	ABCHECK(nullptr != CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}
