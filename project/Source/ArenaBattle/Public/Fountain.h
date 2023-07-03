// Fill out your copyright notice in the Description page of Project Settings.
// 분수대 생성
#pragma once

#include "ArenaBattle.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Fountain.generated.h"

UCLASS()
class ARENABATTLE_API AFountain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// 언제 이벤트 함수를 호출하는지 살펴보기 위해 로그를 찍는다.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 분수대는 분수대 구조물과 물, 각각의 비주얼과 충돌을 담당할 스태틱메시 컴포넌트로 구성되어있다.

	// 포인터를 선언하면 메모리를 관리해주어야 하지만 언리얼 엔진은 실행환경(Runtime)을 통해
	// 객체가 사용되지 않으면 자동으로 메모리를 소멸시키는 기능을 제공한다.
	// UPROPERTY()라는 매크로를 사용해 객체를 지정해주면 선언한 객체를 자동으로 관리해준다.
	// 언리얼 오브젝트라는 특별한 객체에만 사용할 수 있다.
	UPROPERTY(VisibleAnywhere)	// 보기 설정
	UStaticMeshComponent* Body;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Water;

	// 조명
	UPROPERTY(VisibleAnywhere)
	UPointLightComponent* Light;

	// 찰랑이는 이펙트
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* Splash;

	// UPROPERTY 매크로 설정으로 0의 초기 값을 보장받는 멤버 변수
	// EditAnywhere 키워드로 어서든 편집할 수 있게 한다.
	// Category=분류명 규칙으로 지정된 분류엣 ㅓ값을 관리한다.
	UPROPERTY(EditAnywhere, Category=ID)
	int32 ID;

	// 회전 무브먼트 컴포넌트
	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* Movement;

private:
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	float RotateSpeed;
};
