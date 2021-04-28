// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyMinion.generated.h"


UCLASS()
class SOULLIKE_API AEnemyMinion : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AEnemyMinion();

	FVector StartingPose;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FSM_Idle")
	float StartingPoseDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM_FaceOff")
	float FaceOffAngle;

	float EnemyDeltaTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* WeaponCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FAttackInfo> AttackInfos;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FAttackInfo NowAttackInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	float NowAttackCoolTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOutOfCombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundWave* SwingSound;

private:

public:
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

#pragma region Combat	
	virtual void StartAttack() override;
	virtual void EndAttack() override;

	void SetCollision(bool IsOn) override;
	void SetIsAttack(bool Attack) override;

	virtual void SetAttackOtherWeapon(bool IsOn)override;

	virtual void PlayAttackMontage();

	FAttackInfo GetRandomAttackInfo();

	float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

#pragma endregion

	void HitEvent(FVector HitPose) override;

	void StartHitEvent() override;

	void EndHitEvent() override;

	void EndDeathEvent() override;
};