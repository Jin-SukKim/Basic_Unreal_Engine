// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameStateBase.h"
#include "ABGameState.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AABGameState();

	int32 GetTotalGameScore() const;
	void AddGameScore();

	// 게임이 종료되는 두가지 시점(1. 미션 클리어, 2. 플레이어 죽음)
	void SetGameCleared();
	bool IsGameCleared() const;

private:
	// 전체 게임 스코어
	UPROPERTY(Transient)
	int32 TotalGameScore;

	UPROPERTY(Transient)
	bool bGameCleared;
};
