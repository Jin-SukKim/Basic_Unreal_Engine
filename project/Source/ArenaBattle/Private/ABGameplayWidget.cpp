// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameplayWidget.h"
#include "Components/Button.h"
#include "ABPlayerController.h"

// �̸����� ��ư�� ã��, �ش� �̸��� ��ư�� �����ϸ� ���ε� ����
void UABGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnResume")));
	if (nullptr != ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnResumeClicked);
	}

	ReturnToTitleButton = Cast<UButton>(GetWidgetFromName(TEXT("btnReturnToTitle")));
	if (nullptr != ReturnToTitleButton)
	{
		ReturnToTitleButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnReturnToTitleClicked);
	}

	RetryGameButton = Cast<UButton>(GetWidgetFromName(TEXT("btnRetryGame")));
	if (nullptr != RetryGameButton)
	{
		RetryGameButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnRetryGameClicked);
	}
}

void UABGameplayWidget::OnResumeClicked()
{
	// UI�� GetOwningPlayer �Լ��� ����� ���� �ڽ��� �����ϰ� �����ϴ� PlayerController�� ������ ������ �� �ִ�.
	auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer());
	ABCHECK(nullptr != ABPlayerController);

	// ���� ����Ʈ�� ����� �ڽ��� ������ �� �ִ� �Լ�
	RemoveFromParent();

	// �Է� ���� UI���� �������� �ٲ��ش�.
	ABPlayerController->ChangeInputMode(true);
	ABPlayerController->SetPause(false);
}

void UABGameplayWidget::OnReturnToTitleClicked()
{
	// Ÿ��Ʋ�� ���ư��� Title Level�� �����´�.
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}

void UABGameplayWidget::OnRetryGameClicked()
{
	// �ڽ��� �����ϰ� �����ϴ� Controller ������ �����´�.
	auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer());
	ABCHECK(nullptr != ABPlayerController);
	ABPlayerController->RestartLevel();
}
