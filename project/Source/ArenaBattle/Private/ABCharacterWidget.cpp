// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "ABCharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat)
{
	ABCHECK(nullptr != NewCharacterStat);

	CurrentCharacterStat = NewCharacterStat;
	// StatComponent�� �����Ǵ� Delegate
	NewCharacterStat->OnHPChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget);
}

// UI �ý����� �غ�Ǹ� ȣ��Ǵ� �Լ�
void UABCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// ������ ���� �̸����� ������ �˻�
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ABCHECK(nullptr != HPProgressBar);
	// ���� �Ӽ��� ������Ʈ
	UpdateHPWidget();
}

void UABCharacterWidget::UpdateHPWidget()
{
	if (CurrentCharacterStat.IsValid())
	{
		if (nullptr != HPProgressBar)
		{	
			// ���ȿ��� HPRatio�� ������ �Ӽ� ������Ʈ
			HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
		}
	}
}
