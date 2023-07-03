// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABPawn.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameState.h"

AABGameMode::AABGameMode()
{
	// UE의 클래스 정보는 자동으로 생성된 StaticClass라는 함수를 호출해 가져올 수 있다.
	// 액터를 생성하고 지정하는 것이 아닌 클래스 정보를 지정한다. (멀티플레이까지 고려)
	// 멀티플레이어 게임에서는 정보만 저장한 후 플레이어 입장할 때마다 클래스 정보 기반으로
	// 폰을 생성하는 것이 합리적이다.
	//DefaultPawnClass = AABPawn::StaticClass();


	// 제작한 캐릭터를 기본 폰으로 설정
	DefaultPawnClass = AABCharactter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();

	/*
	// C++ 게임 모드에 블루프린트로 제작된 마네킹 폰을 지정
	// 뒤에 '_C'를 붙여서 애셋의 클래스 정보를 가져올 수 있따.
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
	// 게임의 PlayerController 목록을 GetPlayerControllerIterator() 함수로 가져올 수 있다.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
		// 점수를 얻은 플레이어가 누군지 확인
		if ((nullptr != ABPlayerController) && (ScoredPlayer == ABPlayerController))
		{
			// 플레이어 게임 점수 추가
			ABPlayerController->AddGameScore();
			break;
		}
	}


	//ABLOG(Warning, TEXT("Mode : %s"), *ABGameState->GetActorNameOrLabel());
	// 전체 게임 점수 추가
	ABGameState->AddGameScore();

	// 미션 클리어해 게임이 종료되면 동작하는 모든 폰을 멈추고 bGameClear 속성을 true로 설정해준다.
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
				// 게임 클리어되면 UI를 보여준다.
				ABPlayerController->ShowResultUI();
			}
		}
	}
}

int32 AABGameMode::GetScore() const
{
	return ABGameState->GetTotalGameScore();
}
