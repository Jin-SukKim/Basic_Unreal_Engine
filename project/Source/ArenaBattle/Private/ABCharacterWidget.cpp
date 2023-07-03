// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "ABCharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat)
{
	ABCHECK(nullptr != NewCharacterStat);

	CurrentCharacterStat = NewCharacterStat;
	// StatComponent와 연동되는 Delegate
	NewCharacterStat->OnHPChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget);
}

// UI 시스템이 준비되면 호출되는 함수
void UABCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 생성한 위젯 이름으로 위젯을 검색
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ABCHECK(nullptr != HPProgressBar);
	// 위젯 속성을 업데이트
	UpdateHPWidget();
}

void UABCharacterWidget::UpdateHPWidget()
{
	if (CurrentCharacterStat.IsValid())
	{
		if (nullptr != HPProgressBar)
		{	
			// 스탯에서 HPRatio를 가져와 속성 업데이트
			HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
		}
	}
}
