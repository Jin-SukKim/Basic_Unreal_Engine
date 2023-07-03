// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABPawn.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameState.h"

AABGameMode::AABGameMode()
{
	// UE�� Ŭ���� ������ �ڵ����� ������ StaticClass��� �Լ��� ȣ���� ������ �� �ִ�.
	// ���͸� �����ϰ� �����ϴ� ���� �ƴ� Ŭ���� ������ �����Ѵ�. (��Ƽ�÷��̱��� ���)
	// ��Ƽ�÷��̾� ���ӿ����� ������ ������ �� �÷��̾� ������ ������ Ŭ���� ���� �������
	// ���� �����ϴ� ���� �ո����̴�.
	//DefaultPawnClass = AABPawn::StaticClass();


	// ������ ĳ���͸� �⺻ ������ ����
	DefaultPawnClass = AABCharactter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();

	/*
	// C++ ���� ��忡 �������Ʈ�� ���۵� ����ŷ ���� ����
	// �ڿ� '_C'�� �ٿ��� �ּ��� Ŭ���� ������ ������ �� �ֵ�.
	static ConstructorHelpers::FClassFinder<APawn> BP_PAWN_C(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	if (BP_PAWN_C.Succeeded())
	{
		DefaultPawnClass = BP_PAWN_C.Class;
	}
	*/

	PlayerStateClass = AABPlayerState::StaticClass();
	GameStateClass = AABGameState::StaticClass();

	ScoreToClear = 2;
}

void AABGameMode::PostLogin(APlayerController* newPlayer)
{
	ABLOG(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(newPlayer);
	ABLOG(Warning, TEXT("PostLogin End"));

	auto ABPlayerState = Cast<AABPlayerState>(newPlayer->PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	ABPlayerState->InitPlayerData();
}

void AABGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABGameState = Cast<AABGameState>(GameState);
}

void AABGameMode::AddScore(AABPlayerController* ScoredPlayer)
{	
	// ������ PlayerController ����� GetPlayerControllerIterator() �Լ��� ������ �� �ִ�.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
		// ������ ���� �÷��̾ ������ Ȯ��
		if ((nullptr != ABPlayerController) && (ScoredPlayer == ABPlayerController))
		{
			// �÷��̾� ���� ���� �߰�
			ABPlayerController->AddGameScore();
			break;
		}
	}


	//ABLOG(Warning, TEXT("Mode : %s"), *ABGameState->GetActorNameOrLabel());
	// ��ü ���� ���� �߰�
	ABGameState->AddGameScore();

	// �̼� Ŭ������ ������ ����Ǹ� �����ϴ� ��� ���� ���߰� bGameClear �Ӽ��� true�� �������ش�.
	if (GetScore() >= ScoreToClear)
	{
		ABGameState->SetGameCleared();

		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			(*It)->TurnOff();
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
			if (nullptr != ABPlayerController)
			{
				// ���� Ŭ����Ǹ� UI�� �����ش�.
				ABPlayerController->ShowResultUI();
			}
		}
	}
}

int32 AABGameMode::GetScore() const
{
	return ABGameState->GetTotalGameScore();
}
