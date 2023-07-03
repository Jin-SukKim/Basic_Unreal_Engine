// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterSetting.generated.h"

/**
 * 
 */
// �𸮾� ������Ʈ�� �⺻���� INI ���Ͽ��� �ҷ����̱� ���� config Ű���� �߰�
// UCLASS ��ũ�� �� config Ű���忡 �ִ� ArenaBattle�̶�� �������� ����, UE�� �ʱ�ȭ �ܰ迡��
// config ������ ��ġ�� DefaultArenaBattle.ini ������ �о�鿩 CharacterAssets ���� �����Ѵ�.
UCLASS(config=ArenaBattle)
class ARENABATTLESETTING_API UABCharacterSetting : public UObject
{
	GENERATED_BODY()
	
public:
	UABCharacterSetting();
	
	// �ҷ����� PROPERTY �Ӽ��� config Ű���� ������ INI ���Ͽ��� �ش� �Ӽ� ���� �о� �����ϰ� �Ѵ�.
	UPROPERTY(config)
	TArray<FSoftObjectPath> CharacterAssets;
};
