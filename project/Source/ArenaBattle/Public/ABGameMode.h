// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "ABCharactter.h"
#include "GameFramework/GameModeBase.h"
#include "ABGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AABGameMode();

	// 로그인을 완료하면 호출되는 이벤트 함수
	// 폰을 생성하고 컨트롤러의 빙의하는 작업이 이루어진다.
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void PostInitializeComponents() override;
	void AddScore(class AABPlayerController* ScoredPlayer);
	int32 GetScore() const;

private:
	UPROPERTY()
	class AABGameState* ABGameState;

	UPROPERTY()
	int32 ScoreToClear;
};
