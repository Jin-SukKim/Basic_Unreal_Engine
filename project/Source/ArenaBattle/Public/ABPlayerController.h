// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AABPlayerController();

	// �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ����
	virtual void PostInitializeComponents() override;
	// ���Ǹ� �����ϴ� ����
	virtual void OnPossess(APawn* aPawn) override;

	// HUD UI ����
	class UABHUDWidget* GetHUDWidget() const;

	// �÷��̾ NPC�� �׿����� Ȯ��
	void NPCKill(class AABCharactter* KilledNPC) const;

	void AddGameScore() const;

	void ChangeInputMode(bool bGameMode = true);
	// ������ ����Ǹ� UI�� ������.
	void ShowResultUI();
protected:
	// ���� ����
	virtual void BeginPlay() override;

	// HUD ���� �ּ��� Ŭ���� ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABHUDWidget> HUDWidgetClass;

	// �Է� ������Ʈ
	// �÷��̾� ��Ʈ�ѷ��� �Է� ����
	virtual void SetupInputComponent() override;

	UPROPERTY()
	UInputAction* GamePause;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGameplayWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGameplayResultWidget> ResultWidgetClass;
private:
	// �¿� �Է�
	void LeftRight(float NewAxisValue);

	UPROPERTY()
	class UABHUDWidget* HUDWidget;

	UPROPERTY()
	class AABPlayerState* ABPlayerState;

	void OnGamePause();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputMapping;

	UPROPERTY()
	class UABGameplayWidget* MenuWidget;

	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly   UIInputMode;

	UPROPERTY()
	class UABGameplayResultWidget* ResultWidget;
};
