// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "ABGameInstance.generated.h"

// �����͸� �ҷ����̱� ���� ����ü ����
USTRUCT(BlueprintType)
struct FABCharacterData : public FTableRowBase
{
	GENERATED_BODY() // ��ũ��

public:
	FABCharacterData() : Level(1), MaxHP(100.f), Attack(10.f), DropExp(10), NextExp(30) {}

	// �ܺ� ������ ������ �� ���� �̸��� ������ ������ Ÿ�Կ� ���� �������ش�.
	// ù ��° �� �����ʹ� UE���� �ڵ����� Ű ������ ���� ���ܵȴ�.
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

	// ���� �� �ʱ�ȭ�� ȣ��Ǵ� �Լ�
	virtual void Init() override;

	// ������ ���� ������ ���
	FABCharacterData* GetABCHaracterData(int32 Level);

	// ���� ���� �߿��� �񵿱� ������� �ּ��� �ε�
	FStreamableManager StreamableManager;

private:
	// ���̺� �����͸� �����ϴ� �𸮾� ������Ʈ
	UPROPERTY()
	class UDataTable* ABCharacterTable;
};
