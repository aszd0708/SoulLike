// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class SOULLIKE_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	class USphereComponent* HitCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	float BulletDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	TSubclassOf<UDamageType> DamageTypeClass;

	class AActor* ShootActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FireInDirection(FVector FireDirection);

	void SetDamage(float Damage);

	void SetShootActor(AActor* Actor);

	UFUNCTION()
	void BeginOnOverlapBulletSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
