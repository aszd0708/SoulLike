// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeathBall.generated.h"

UCLASS()
class SOULLIKE_API ADeathBall : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADeathBall();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	class USphereComponent* HitCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
		AActor* TargetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
		float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		TSubclassOf<UDamageType> DamageTypeClass;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float UpZPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float DestroyTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
		float NowTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MoveToTarget(float DeltaTime);

	UFUNCTION()
		void BeginOnOverlapAttackSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetBallObject(AActor* Target, float Damage);

};
