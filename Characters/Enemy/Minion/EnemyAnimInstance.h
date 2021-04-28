// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;	
	
	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	virtual void UpdateAniationProprties();

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Movement)
	float MovementYaw;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Movement)
	class APawn* Pawn;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Movement)
	class AEnemyBase* Enemy;
};
