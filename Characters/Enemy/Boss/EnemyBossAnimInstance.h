// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAnimInstance.h"
#include "EnemyBossAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UEnemyBossAnimInstance : public UEnemyAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;

	virtual void UpdateAniationProprties() override;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Character")
	class AEnemyBoss* EnemyBoss;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Character")
	float DashSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Character")
	float MoveToTargetSpeed;
};
