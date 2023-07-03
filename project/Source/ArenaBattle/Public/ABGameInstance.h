// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "ABGameInstance.generated.h"

// 데이터를 불러들이기 위한 구조체 선언
USTRUCT(BlueprintType)
struct FABCharacterData : public FTableRowBase
{
	GENERATED_BODY() // 매크로

public:
	FABCharacterData() : Level(1), MaxHP(100.f), Attack(10.f), DropExp(10), NextExp(30) {}

	// 외부 데이터 파일의 각 열의 이름과 동일한 변수를 타입에 맞춰 선언해준다.
	// 첫 번째 열 데이터는 UE에서 자동으로 키 값으로 사용돼 제외된다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 DropExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 NextExp;
};

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UABGameInstance();

	// 게임 앱 초기화시 호출되는 함수
	virtual void Init() override;

	// 레벨에 따른 데이터 얻기
	FABCharacterData* GetABCHaracterData(int32 Level);

	// 게임 진행 중에도 비동기 방식으로 애셋을 로딩
	FStreamableManager StreamableManager;

private:
	// 테이블 데이터를 관리하는 언리얼 오브젝트
	UPROPERTY()
	class UDataTable* ABCharacterTable;
};
