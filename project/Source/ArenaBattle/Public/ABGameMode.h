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

	// �α����� �Ϸ��ϸ� ȣ��Ǵ� �̺�Ʈ �Լ�
	// ���� �����ϰ� ��Ʈ�ѷ��� �����ϴ� �۾��� �̷������.
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
