// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABItemBox.generated.h"

UCLASS()
class ARENABATTLE_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItemBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = Box)
	UBoxComponent* Trigger;
	
	UPROPERTY(VisibleAnywhere, Category = Box)
	UStaticMeshComponent* Box;

	// TSubclassOf는 특정 클래스와 상속받은 클래스들로 목록을 한정할 수 있다.
	// 즉 모든 UClass(언리얼 오브젝트)가 보이는게 아닌 아이템 상자와 이를 선언한
	// 클래스 목록인 웨폰 클래스만 볼 수 있다.
	UPROPERTY(EditInstanceOnly, Category = Box)
	TSubclassOf<class AABWeapon> WeaponItemClass;

	// 이펙트
	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent* Effect;

private:

	// 충돌 시 호출되는 함수, 델리게이트에 상요하므로 UFUNCTION 매크로를 사용한다.
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherOBdyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// 파티클 컴포넌트에서 제공하는 델리게이트를 연동해 이펙트 재생이 종료시 액터가 제거되도록 한다.
	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* PSystem);

};
