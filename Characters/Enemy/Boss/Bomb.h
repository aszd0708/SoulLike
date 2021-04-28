// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bomb.generated.h"

UCLASS()
class SOULLIKE_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABomb();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosive")
	float ExplosiveRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosive")
	UParticleSystem* ExplosiveParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosive")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosive")
	class USoundWave* ExplosionSound;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Explosion(float Damage);
};
