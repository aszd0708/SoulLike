// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UMainPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAniationProprties();

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Movement)
	float MovementYaw;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Combat")
	float HitDirectionX;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Combat")
	float HitDirectionY;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Movement)
	class APawn* Pawn;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Movement)
	class AMainPlayer* MainPlayer;
};
