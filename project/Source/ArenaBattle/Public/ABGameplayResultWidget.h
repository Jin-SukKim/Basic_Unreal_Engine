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
	// NativeContruct 함수는 AddToViewport 함수가 외부에서 호출될 떄 UI 위젯이 초기화되며 호출된다.
	// 그래서 PlayerController에서 ShowResultUI 안에 AddToViewport 함수 호출 전
	// 미리 UI 위젯이 GameState 정보를 읽도록 바인딩한다.
	void BindGameState(class AABGameState* GameState);
protected:
	virtual void NativeConstruct() override;

private:
	TWeakObjectPtr<class AABGameState> CurrentGameState;
};
