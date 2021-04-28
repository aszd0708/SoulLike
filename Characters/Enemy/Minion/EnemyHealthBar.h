// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UEnemyHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void NativeConstruct() override;

	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* HealthBar;
	
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Enemies")
	class AEnemyBase* Enemy;

public:

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetEnemyHealth();

	UFUNCTION(BlueprintCallable)
	AEnemyBase* GetEnemy(int32 Index);

	UFUNCTION(BlueprintCallable)
	void SetProgressBarPercent(AEnemyBase* TargetEnemy);
};
