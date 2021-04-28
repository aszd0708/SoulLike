// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHealthBar.h"
#include "EnemyBase.h"
#include "MainPlayer.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Components/SizeBox.h"

void UEnemyHealthBar::NativeConstruct()
{
	HealthBar = Cast<UProgressBar>(GetWidgetFromName("HealthBar"));
}

void UEnemyHealthBar::SetProgressBarPercent(AEnemyBase* TargetEnemy)
{
	float HealthPercent = 0;

	if (TargetEnemy)
	{
		float NowHealth = TargetEnemy->CharacterStats.Health;
		float MaxHealth = TargetEnemy->CharacterStats.MaxHealth;

		HealthPercent = NowHealth / MaxHealth;
	}
	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercent);
	}

	if (HealthPercent <= 0)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

float UEnemyHealthBar::GetEnemyHealth()
{
	float HealthPercent = 0;

	if (Enemy)
	{
		float NowHealth = Enemy->CharacterStats.Health;
		float MaxHealth = Enemy->CharacterStats.MaxHealth;

		HealthPercent = NowHealth / MaxHealth;
	}
	UE_LOG(LogTemp, Log, TEXT("%f"), HealthPercent);

	return HealthPercent;
}

AEnemyBase* UEnemyHealthBar::GetEnemy(int32 Index)
{
	APawn* PlayerPawn = GetOwningPlayerPawn();
	AEnemyBase* TargetEnemy = nullptr;
	AMainPlayer* Player = Cast<AMainPlayer>(PlayerPawn);
	if (Player)
	{
		if (Player->Enemise[Index])
		{
			TargetEnemy = Player->Enemise[Index];
		}
	}

	return TargetEnemy;
}