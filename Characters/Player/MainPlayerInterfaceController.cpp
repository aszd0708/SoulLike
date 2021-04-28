// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerInterfaceController.h"
#include "Blueprint/UserWidget.h"
#include "MainPlayer.h"
#include "Kismet/GameplayStatics.h"

void AMainPlayerInterfaceController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}

	if (HUDOverlay)
	{
		HUDOverlay->AddToViewport();
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
	}

	if (WEnemyHealthBar)
	{
		for (int i = 0; i < 3; i++)
		{
			UEnemyHealthBar* EnemyHealthBar = CreateWidget<UEnemyHealthBar>(this, WEnemyHealthBar);
			if (EnemyHealthBar)
			{
				EnemyHealthBar->AddToViewport();
				EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
			}
			FVector2D Alignment(0.f, 0.f);
			EnemyHealthBar->SetAlignmentInViewport(Alignment);
			EnemyHealthBars.Add(EnemyHealthBar);
		}
	}
}

void AMainPlayerInterfaceController::Tick(float DeltaTime)
{
	GetEnemies();
	if (EnemyHealthBars.Num() != 0 && Enemies.Num() != 0)
	{
		for (int i = 0; i < Enemies.Num() && i < EnemyHealthBars.Num(); i++)
		{
			if (Enemies[i])
			{
				if (!Enemies[i]->bIsDead)
				{
					FVector2D PositionInViewport;
					ProjectWorldLocationToScreen(Enemies[i]->GetActorLocation(), PositionInViewport);
					PositionInViewport.Y -= 85.f;

					FVector2D SizeInViewport(300.f, 25.f);

					EnemyHealthBars[i]->SetPositionInViewport(PositionInViewport);
					EnemyHealthBars[i]->SetDesiredSizeInViewport(SizeInViewport);
					EnemyHealthBars[i]->SetVisibility(ESlateVisibility::Visible);

					//EnemyHealthBars[i]->Enemy = Enemies[i];
					EnemyHealthBars[i]->SetProgressBarPercent(Enemies[i]);
				}
				else
				{
					EnemyHealthBars[i]->SetVisibility(ESlateVisibility::Hidden);
				}
			}

			else
			{
				EnemyHealthBars[i]->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

}

void AMainPlayerInterfaceController::GetEnemies()
{
	APawn* PlayerPawn = GetPawn();
	if (PlayerPawn)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(PlayerPawn);
		if (MainPlayer)
		{
			Enemies = MainPlayer->Enemise;

		}
	}
}

void AMainPlayerInterfaceController::DisplayEnemyHealthBar()
{
	//if (EnemyHealthBar)
	//{
	//	bEnemyHealthBarVibivle = true;
	//	EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	//}
}

void AMainPlayerInterfaceController::RemoveEnemyHealthBar()
{
	//if (EnemyHealthBar)
	//{
	//	bEnemyHealthBarVibivle = false;
	//	EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	//}
}

void AMainPlayerInterfaceController::GameModeOnly()
{
	FInputModeGameOnly InputModeGameOnly;

	SetInputMode(InputModeGameOnly);
}