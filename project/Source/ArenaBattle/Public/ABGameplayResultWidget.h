// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "ABGameplayWidget.h"
#include "ABGameplayResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameplayResultWidget : public UABGameplayWidget
{
	GENERATED_BODY()
	
public:
	// NativeContruct �Լ��� AddToViewport �Լ��� �ܺο��� ȣ��� �� UI ������ �ʱ�ȭ�Ǹ� ȣ��ȴ�.
	// �׷��� PlayerController���� ShowResultUI �ȿ� AddToViewport �Լ� ȣ�� ��
	// �̸� UI ������ GameState ������ �е��� ���ε��Ѵ�.
	void BindGameState(class AABGameState* GameState);
protected:
	virtual void NativeConstruct() override;

private:
	TWeakObjectPtr<class AABGameState> CurrentGameState;
};
