#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyBoss.generated.h"

UENUM(BlueprintType)
enum class EBossMovementStatus : uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	EMS_FaceOff			UMETA(DisplayName = "FaceOff"),
	EMS_Attack			UMETA(DisplayName = "Attack"),
	EMS_Dash			UMETA(DisplayName = "Dash"),
	EMS_Dead			UMETA(DisplayName = "Dead"),
	 
	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EDistanceInfomation : uint8
{
	EDI_Almost			UMETA(DisplayName = "Almost"),
	EDI_Middle			UMETA(DisplayName = "Middle"),
	EDI_Range			UMETA(DisplayName = "Range"),

	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FBossAttackInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float RecoverTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float NowAttackCoolTime;
};


UCLASS()
class SOULLIKE_API AEnemyBoss : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AEnemyBoss();

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	FBossAttackInfo* NowAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
	TSubclassOf<UCameraShake> CameraShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText Name;

#pragma region Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveToTargetSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float FaceOffSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EBossMovementStatus NowMovementStatus;

	EBossMovementStatus PrevMovementStatus;

	bool bIsAttack;
	EBossMovementStatus AfterAttackChangeStatus;
#pragma endregion

#pragma region AttackInfos
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackInfo")
	FBossAttackInfo Attack_OneHitCircle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackInfo")
	FBossAttackInfo Attack_OneHitUltimate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackInfo")
	FBossAttackInfo Attack_ThreeAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackInfo")
	FBossAttackInfo Attack_EarthQuake;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackInfo")
	FBossAttackInfo Attack_SummonBomb;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackInfo")
	FBossAttackInfo Attack_DeathBall;

	// 비어있는 구조체
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackInfo")
	FBossAttackInfo Attack_Empty;
#pragma endregion

#pragma region Dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	UAnimMontage* DashMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
	float NowDashCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashCoolTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
	bool bIsDash;
#pragma endregion

#pragma region Hit

#pragma endregion

#pragma region Attack
#pragma region EarthQuake

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EarthQuake")
	float EarthQukeExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EarthQuake")
	class UParticleSystem* EarthQuakeParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EarthQuake")
	class USoundWave* EarthQuakeSound;

#pragma endregion

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	class USoundWave* SwingSound;
#pragma region SummonBomb

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SummonBomb")
	class TSubclassOf<AActor> Bomb;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SummonBomb")
	class ABomb* NowBomb;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SummonBomb")
	class UParticleSystem* BombSummonEffect;
#pragma endregion

#pragma region DeathBall

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeathBall")
	class TSubclassOf<AActor> DeathBall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeathBall")
	class UParticleSystem* DeathBallShootEffect;

#pragma endregion

#pragma endregion

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	class UBoxComponent* WeaponCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float NowRecoverTime;

	// 컴벳 -> 근거리 대치 -> 중거리 어그로 -> 원거리
	bool bIsAlMost, bIsMiddle, bIsRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TargetDistance")
	EDistanceInfomation NowAttackDistance;

	FORCEINLINE void SetNowAttackDistance(EDistanceInfomation AttackDistance) { NowAttackDistance = AttackDistance; }
public:
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

#pragma region Hit
	float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void HitEvent(FVector HitPose) override; 
#pragma endregion

#pragma region FSM
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	AActor* NowFSMTarget;

	void FSM_Boss(AActor* Target = nullptr);

	void FSMEnter_Boss(AActor* Target = nullptr);
	void FSMUpdate_Boss(AActor* Target = nullptr);
	void FSMExit_Boss(EBossMovementStatus Status);
	void FSMSet_Boss(EBossMovementStatus Status, AActor* Target = nullptr);

#pragma region Idle
	void IdleEnter_Boss();
	void IdleUpdate_Boss();
	void IdleExit_Boss();
#pragma endregion 

#pragma region MoveToTarget
	void MoveToTargetEnter_Boss(AActor* Target);
	void MoveToTargetUpdate_Boss(AActor* Target);
	void MoveToTargetExit_Boss();
#pragma endregion 

#pragma region FaceOff
	void FaceOffEnter_Boss(AActor* Target);
	void FaceOffUpdate_Boss(AActor* Target);
	void FaceOffExit_Boss();
#pragma endregion 

#pragma region Attack
	void AttackEnter_Boss(AActor* Target);
	void AttackUpdate_Boss(AActor* Target);
	void AttackExit_Boss();
#pragma endregion

#pragma region Dash
	void DashEnter_Boss(AActor* Target);
	void DashUpdate_Boss(AActor* Target);
	void DashExit_Boss();
#pragma endregion 
#pragma endregion

#pragma region Collisions
	virtual void BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void BeginOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void EndOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void BeginOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void EndOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void BeginOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void EndOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

#pragma endregion 

#pragma region Pattern
	//FBossAttackInfo* GetAttackInfo();

	void SetAttackInfo();
	
	bool CheckAttackCoolTime(FBossAttackInfo& AttackInfo);

	void SetBossAttackNowCoolTimes(float DeltaTime);

	void PlayAttackMontage();

#pragma endregion


#pragma region Attack
	void StartAttack() override;

	void EndAttack() override;

	UFUNCTION(BlueprintCallable)
	void TrunWeaponCollider(bool IsOn);

#pragma region EarthQuake

	UFUNCTION(BlueprintCallable)
	void EarthQuakeExplosion();

#pragma endregion 

#pragma region SummonBomb

	UFUNCTION(BlueprintCallable)
	void SummonBomb();

	UFUNCTION(BlueprintCallable)
	void ExplosionBomb();

#pragma endregion

#pragma region DeathBall

	UFUNCTION(BlueprintCallable)
	void ShootDeathBall();

#pragma endregion

#pragma endregion

};
