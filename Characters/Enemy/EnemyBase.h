// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulLikeCharacterBase.h"
#include "EnemyBase.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	EMS_FaceOff			UMETA(DisplayName = "FaceOff"),
	EMS_Attack			UMETA(DisplayName = "Attack"),
	EMS_Hit				UMETA(DisplayName = "Hit"),
	EMS_Dead			UMETA(DisplayName = "Dead"),

	EMS_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EFSMStatus : uint8
{
	EFS_Enter			UMETA(DisplayName = "Enter"),
	EFS_Update			UMETA(DisplayName = "Update"),
	EFS_Exit			UMETA(DisplayName = "Exit"),

	EMS_MAX			UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FAttackInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		FName AttackName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float AttackCoolTime;
};

UCLASS()
class SOULLIKE_API AEnemyBase : public ASoulLikeCharacterBase
{
	GENERATED_BODY()
	
public :
	AEnemyBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	EFSMStatus FSMStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyMovementStatus PrevMovementStatus;

	FORCEINLINE EFSMStatus GetFSMStatus() { return FSMStatus; }

	FORCEINLINE EEnemyMovementStatus GetMovementStatus() { return MovementStatus; }
	FORCEINLINE void SetMovementStatus(EEnemyMovementStatus Movement) { MovementStatus = Movement; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UCapsuleComponent* CombatCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Agro")
	class USphereComponent* AgroCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FaceOff")
	class USphereComponent* FaceOffCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Agro")
	float MoveToTargetMovmentSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FaceOff")
	float FaceOffMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
	class UAnimMontage* HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death")
	class UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
	FVector HitDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AActor* FSMTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death")
	bool bIsDead;

protected:
	virtual void BeginPlay() override;


public:
	virtual void Tick(float DeltaTime) override;

#pragma region FSM

	virtual void FSM(AActor* Target = nullptr);

	virtual void FSMEnter(AActor* Target = nullptr);
	virtual void FSMUpdate(AActor* Target = nullptr);
	virtual void FSMExit(EEnemyMovementStatus Status);
	virtual void FSMSet(EEnemyMovementStatus Status, AActor* Target = nullptr);

#pragma region Idle
	virtual void IdleEnter();
	virtual void IdleUpdate();
	virtual void IdleExit();
#pragma endregion 

#pragma region MoveToTarget
	virtual void MoveToTargetEnter(AActor* Target);
	virtual void MoveToTargetUpdate(AActor* Target);
	virtual void MoveToTargetExit();
#pragma endregion 

#pragma region FaceOff
	virtual void FaceOffEnter(AActor* Target);
	virtual void FaceOffUpdate(AActor* Target);
	virtual void FaceOffExit();
#pragma endregion 

#pragma region Attack
	virtual void AttackEnter(AActor* Target);
	virtual void AttackUpdate(AActor* Target);
	virtual void AttackExit();
#pragma endregion 

#pragma region Hit
	virtual void HitEnter();
	virtual void HitUpdate();
	virtual void HitExit();
#pragma endregion 


#pragma region Dead
	virtual void DeadEnter();
	virtual void DeadUpdate();
	virtual void DeadExit();
#pragma endregion 


#pragma endregion 

#pragma region Collisions
	virtual void BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	virtual void BeginOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void BeginOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void BeginOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#pragma endregion 

#pragma region Combat
	UFUNCTION(BlueprintCallable)
	virtual	void StartAttack();
	UFUNCTION(BlueprintCallable)
	virtual	void EndAttack();
	UFUNCTION(BlueprintCallable)
	virtual	void SetCollision(bool IsOn);
	UFUNCTION(BlueprintCallable)
	virtual void SetAttackOtherWeapon(bool IsOn);

	UFUNCTION(BlueprintCallable)
	void SetDualAttackCollision(bool IsOn);
	UFUNCTION(BlueprintCallable)
	virtual void SetIsAttack(bool Attack);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void HitEvent(FVector HitPose) override;
#pragma endregion

#pragma region Hit
	UFUNCTION(BlueprintCallable)
	virtual void StartHitEvent();

	UFUNCTION(BlueprintCallable)
	virtual void EndHitEvent();
#pragma endregion

	UFUNCTION(BlueprintCallable)
	virtual void EndDeathEvent();
};
