// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerState.h"
#include "ABPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangedDelegate);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AABPlayerState();

	// 게임 스코어
	int32 GetGameScore() const;
	int32 GetCharacterLevel() const; 
	int32 GetCharacterIndex() const;

	void InitPlayerData();

	// Player Data와 연동을 위한 Delegate
	FOnPlayerStateChangedDelegate OnPlayerStateChanged;

	// 경험치
	float GetExpRatio() const;
	bool AddExp(int32 IncomeExp);

	void AddGameScore();

	int32 GetGameHighScore() const;
	// 저장된 게임 데이터 파일에 접근하기 위한 고유의 슬롯 이름
	FString SaveSlotName;

	// 플레이어 데이터 저장
	void SavePlayerData();

	
protected:
	UPROPERTY(Transient)
	int32 GameScore;

	UPROPERTY(Transient)
	int32 CharacterLevel;

	UPROPERTY(Transient)
	int32 Exp;

	UPROPERTY(Transient)
	int32 GameHighScore;

	UPROPERTY(Transient)
	int32 CharacterIndex;
private:
	void SetCharacterLevel(int32 NewCharacterLevel);
	struct FABCharacterData* CurrentStatData;
};
