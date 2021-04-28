// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulLikeCharacterBase.h"
#include "EnemyBase.h"
#include "EnemyBoss.h"
#include "MainPlayer.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal		UMETA(DisplayName = "Normal"),
	EMS_Sprinting	UMETA(DisplayName = "Sprinting"),
	EMS_Dead		UMETA(DisplayName = "Dead"),
	EMS_Hit			UMETA(DisplayName = "Hit"),

	EMS_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EBehaviorStatus : uint8
{
	EBS_Normal		UMETA(DisplayName = "Normal"),
	EBS_Avoiding	UMETA(DisplayName = "Avoiding"),
	EBS_Attacking	UMETA(DisplayName = "Attacking"),
	EBS_Dead		UMETA(DisplayName = "Dead"),

	EMS_MAX			UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SOULLIKE_API AMainPlayer : public ASoulLikeCharacterBase
{
	GENERATED_BODY()
	
public:
	AMainPlayer();	

	float PlayerDeltaTime;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPreserveRatio = "true"))
	class USpringArmComponent* CameraBoom;

	// 카메라를 따라다님
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPreserveRatio = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class USphereComponent* EnemyTargetCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetSystem")
	class UTargetSystemComponent* TargetSystem;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus PlayerMovementStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enums")
	EBehaviorStatus PlayerBehaviorStatus;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingStamina;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "UI")
	TArray<AEnemyBase*> Enemise;

#pragma region Rolling
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rolling")
	float RollingButtonPushTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling")
	float NowRollingButtonPushTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling")
	float RollingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling")
	float RollingHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling")
	class UAnimMontage* RollingMontage;

	bool bShiftKeyDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling")
	bool bIsRolling;

	bool checkRolling;

	bool bIsLanding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling")
	float RollingStamina;

	FORCEINLINE void SetIsRolling(bool IsRolling) { bIsRolling = IsRolling; }
	FORCEINLINE bool GetIsRolling() { return bIsRolling; }
#pragma endregion

#pragma region Attack

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsAttacking;

	bool bDoComboAttack;

	bool bCheckComboAttack;

	bool bPressedLMB;

	int32 NowAttackIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* LeftWeaponCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* RightWeaponCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundWave* AttackSound;

	TArray<AEnemyBase*> AttackObjects;
	
#pragma endregion

#pragma region Hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
	FVector HitDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
	class UAnimMontage* HitMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hit")
	bool bIsHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
	float HitRecoverTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hit")
	float NowHitRecoverTime;
#pragma endregion

#pragma region Targeting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	bool bIsLockOn;
	int TargetIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	float TargetingRightValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	float TargetingForwardValue;
#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	class AEnemyBoss* Boss;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetPlayerMovementStatus(EMovementStatus MovementStatus) { PlayerMovementStatus = MovementStatus; }
	FORCEINLINE EMovementStatus GetPlayerMovementStatus() { return PlayerMovementStatus; }

	FORCEINLINE void SetPlayerBehaviorStatus(EBehaviorStatus BehaviorStatus) { PlayerBehaviorStatus = BehaviorStatus; }
	FORCEINLINE EBehaviorStatus GetPlayerBehaviorStatus() { return PlayerBehaviorStatus; }

	void CheckMovementStatus();
	void CheckBehaviorStatus();

#pragma region KeyBinding
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Movement
	void Turn(float Value);

	void LookUp(float Value);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);
#pragma endregion 

#pragma region Action
	void PushShift();

	void PullShift();

	void Rolling(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void EndRolling();

	void CheckLanding();

	void CheckRollingButtonTime(float DeltaTime);

	void Attack();

	UFUNCTION(BlueprintCallable)
	void OffAttackCollision();

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	UFUNCTION(BlueprintCallable)
	void DoNextAttack();

	void CheckComboAttack();

#pragma endregion 

#pragma region Targeting

	void SetTarget();

	int GetTargetIndex();

	void ChangeTargetLeft();

	void ChangeTargetRight();

#pragma endregion

#pragma endregion 

	void GetDamage(float Damage) override;

	void SetWeaponCollider();

	void BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void HitEvent(FVector HitEvent) override;

	UFUNCTION(BlueprintCallable)
	void EndHitEvent(); 
	UFUNCTION(BlueprintCallable)
	void StartHitEvent();

	UFUNCTION()
		void BeginOnOverlapEnemyTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void EndOnOverlapEnemyTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void LockOnEnemy();

	UFUNCTION()
	void LockOffEnemy();

	void SwitchLevel(FName LevelName);

	void SetBoss(AEnemyBoss* _Boss);
};
