// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterSetting.generated.h"

/**
 * 
 */
// 언리얼 오브젝트의 기본값을 INI 파일에서 불러들이기 위해 config 키워드 추가
// UCLASS 매크로 내 config 키워드에 있는 ArenaBattle이라는 설정으로 인해, UE은 초기화 단계에서
// config 폴더에 위치한 DefaultArenaBattle.ini 파일을 읽어들여 CharacterAssets 값을 설정한다.
UCLASS(config=ArenaBattle)
class ARENABATTLESETTING_API UABCharacterSetting : public UObject
{
	GENERATED_BODY()
	
public:
	UABCharacterSetting();
	
	// 불러들일 PROPERTY 속성에 config 키워드 선언해 INI 파일에서 해당 속성 값을 읽어 설정하게 한다.
	UPROPERTY(config)
	TArray<FSoftObjectPath> CharacterAssets;
};
