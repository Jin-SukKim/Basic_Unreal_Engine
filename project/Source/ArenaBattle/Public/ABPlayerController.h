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

	// 플레이어 컨트롤러가 생성되는 시점
	virtual void PostInitializeComponents() override;
	// 빙의를 진행하는 시점
	virtual void OnPossess(APawn* aPawn) override;

	// HUD UI 위젯
	class UABHUDWidget* GetHUDWidget() const;

	// 플레이어가 NPC를 죽였는지 확인
	void NPCKill(class AABCharactter* KilledNPC) const;

	void AddGameScore() const;

	void ChangeInputMode(bool bGameMode = true);
	// 게임이 종료되면 UI를 띄워즌다.
	void ShowResultUI();
protected:
	// 게임 시작
	virtual void BeginPlay() override;

	// HUD 위젯 애셋의 클래스 정보
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABHUDWidget> HUDWidgetClass;

	// 입력 컴포넌트
	// 플레이어 컨트롤러에 입력 매핑
	virtual void SetupInputComponent() override;

	UPROPERTY()
	UInputAction* GamePause;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGameplayWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGameplayResultWidget> ResultWidgetClass;
private:
	// 좌우 입력
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
