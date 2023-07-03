// Fill out your copyright notice in the Description page of Project Settings.


#include "ABUIPlayerController.h"
#include "Blueprint/UserWidget.h"

void AABUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ABCHECK(nullptr != UIWidgetClass);

	// UI �ν��Ͻ� ����
	UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
	ABCHECK(nullptr != UIWidgetInstance);

	// ����Ʈ�� UI �ν��Ͻ� ����
	UIWidgetInstance->AddToViewport();

	// �Է��� UI���� ���޵ǵ��� �Ѵ�.
	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	bShowMouseCursor = true;
}
