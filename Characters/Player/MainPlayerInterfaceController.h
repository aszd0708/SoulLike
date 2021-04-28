// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "BossHealthBar.h"
#include "EnemyHealthBar.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerInterfaceController.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API AMainPlayerInterfaceController : public APlayerController
{
	GENERATED_BODY()
	
public:

	/* Reference to the UMG asset in the editer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;

	/* Variable to hold the widget after creating it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UEnemyHealthBar> WEnemyHealthBar;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Widgets")
	UEnemyHealthBar* EnemyHealthBarBluePrint;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Enemies")
	TArray<UEnemyHealthBar*> EnemyHealthBars;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Enemies")
	TArray<AEnemyBase*> Enemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	UBossHealthBar* BossHealthBar;

	AEnemyBoss* Boss;

	bool bEnemyHealthBarVibivle;
	void DisplayEnemyHealthBar();
	void RemoveEnemyHealthBar();
	bool bPauseMenuVisible;

	FVector EnemyLocation;

	void GameModeOnly();

	void GetEnemies();

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
