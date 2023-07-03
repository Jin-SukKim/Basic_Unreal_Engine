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
	// �� ó�� ���̺� ������ ������ ������ �⺻ ���̺� ���� ����
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
	// ���ʿ� �÷��̾� ������ �������ڸ��� �ٷ� ����
	SavePlayerData();
}

float AABPlayerState::GetExpRatio() const
{
	// ���� �� �ִ� Exp�� 0�� ����� ���
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
		return 0.f;

	// UI�� ProgressBar�� ǥ�õǹǷ� Ratio ���
	float Result = (float)Exp / (float)CurrentStatData->NextExp;
	ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
	return Result;
}

bool AABPlayerState::AddExp(int32 IncomeExp)
{
	if (CurrentStatData->NextExp == -1)
		return false;

	bool DidLevelUp = false;
	// ���� Exp�� ���� Exp �����ֱ�
	Exp = Exp + IncomeExp;
	// ����� ����ġ�� ����ٸ� ������
	if (Exp >= CurrentStatData->NextExp)
	{
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(CharacterLevel + 1);
		DidLevelUp = true;
	}

	OnPlayerStateChanged.Broadcast();
	// ����ġ ȹ���� ������ ������ ����
	SavePlayerData();
	return DidLevelUp;
}

void AABPlayerState::AddGameScore()
{
	GameScore++;
	// �ְ��� ����
	if (GameScore >= GameHighScore)
	{
		GameHighScore = GameScore;
	}
	OnPlayerStateChanged.Broadcast();
	// ���� ȹ���� ������ ������ ����
	SavePlayerData();
}

int32 AABPlayerState::GetGameHighScore() const
{
	return GameHighScore;
}

void AABPlayerState::SavePlayerData()
{
	// ���ο� ���̺� ������ �𸮾� ������Ʈ
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
	NewPlayerData->PlayerName = GetPlayerName();
	NewPlayerData->Level = CharacterLevel;
	NewPlayerData->Exp = Exp;
	NewPlayerData->HighScore = GameHighScore;

	// Slot Name�� �������� slot�� ������ �����ϱ�
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
