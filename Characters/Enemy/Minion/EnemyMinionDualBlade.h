// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyMinion.h"
#include "EnemyMinionDualBlade.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API AEnemyMinionDualBlade : public AEnemyMinion
{
	GENERATED_BODY()
	
public:
	AEnemyMinionDualBlade();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* OtherWeaponCollision;

private:

	virtual void BeginPlay() override;

public:
	void Tick(float DeltaTime) override;
	void SetAttackOtherWeapon(bool IsOn) override;
	void StartAttack() override;
	void EndAttack() override; 
	
	void BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};									
