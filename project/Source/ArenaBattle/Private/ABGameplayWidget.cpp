// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameplayWidget.h"
#include "Components/Button.h"
#include "ABPlayerController.h"

// 이름으로 버튼을 찾고, 해당 이름의 버튼이 존재하면 바인딩 구현
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
	// UI는 GetOwningPlayer 함수를 사용해 현재 자신을 생성하고 관리하는 PlayerController의 정보를 가져올 수 있다.
	auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer());
	ABCHECK(nullptr != ABPlayerController);

	// 현재 뷰포트에 띄워진 자신을 제거할 수 있는 함수
	RemoveFromParent();

	// 입력 모드는 UI에서 게임으로 바꿔준다.
	ABPlayerController->ChangeInputMode(true);
	ABPlayerController->SetPause(false);
}

void UABGameplayWidget::OnReturnToTitleClicked()
{
	// 타이틀로 돌아가게 Title Level을 가져온다.
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}

void UABGameplayWidget::OnRetryGameClicked()
{
	// 자신을 생성하고 관리하는 Controller 정보를 가져온다.
	auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer());
	ABCHECK(nullptr != ABPlayerController);
	ABPlayerController->RestartLevel();
}
