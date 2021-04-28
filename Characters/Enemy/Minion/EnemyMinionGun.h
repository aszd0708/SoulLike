// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyMinionGun.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API AEnemyMinionGun : public AEnemyBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	TSubclassOf<AActor> Bullet;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	class UParticleSystem* ShootParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	float BulletDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	float ShootCooltime;
	float NowShootCooltime;

	FVector StartingPose;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FSM_Idle")
	float StartingPoseDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	class USoundWave* ShootSound;

	bool bOutOfCombatCollision;

	bool bIsAttacking;
public:
	AEnemyMinionGun(); private:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

#pragma region FSM

#pragma region Idle
	void IdleEnter() override;
	void IdleUpdate() override;
	void IdleExit() override;
#pragma endregion 

#pragma region MoveToTarget
	void MoveToTargetEnter(AActor* Target) override;
	void MoveToTargetUpdate(AActor* Target) override;
	void MoveToTargetExit() override;
#pragma endregion 

#pragma region FaceOff
	void FaceOffEnter(AActor* Target) override;
	void FaceOffUpdate(AActor* Target) override;
	void FaceOffExit() override;
#pragma endregion 

#pragma region Attack
	void AttackEnter(AActor* Target) override;
	void AttackUpdate(AActor* Target) override;
	void AttackExit() override;
#pragma endregion 

#pragma region Hit
	void HitEnter() override;
	void HitUpdate() override;
	void HitExit() override;
#pragma endregion 


#pragma region Dead
	void DeadEnter() override;
	void DeadUpdate() override;
	void DeadExit() override;
#pragma endregion 

#pragma endregion 

#pragma region Collisions
	void BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void BeginOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void EndOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void BeginOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void EndOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void BeginOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void EndOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

#pragma endregion
	
	void StartAttack() override;

	void EndAttack() override;

#pragma region Hit
	float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void HitEvent(FVector HitPose) override;

	void StartHitEvent() override;

	void EndHitEvent() override;
#pragma endregion

	void EndDeathEvent() override;

	void SetIsAttack(bool Attack) override;
};
