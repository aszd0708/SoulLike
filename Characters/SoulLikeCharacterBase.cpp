// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulLikeCharacterBase.h"

ASoulLikeCharacterBase::ASoulLikeCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	StaminaChargingValue = 10;
	StaminaChargingTime = 2;
	NowStaminaChargingTime = 0;
}

void ASoulLikeCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASoulLikeCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckStaminaTime(DeltaTime);
}

// Called to bind functionality to input
void ASoulLikeCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


bool ASoulLikeCharacterBase::MinusStamina(float Value)
{
	bool CanUseStamina = false;
	if (CharacterStats.Stamina > 0)
	{
		NowStaminaChargingTime = 0;
		CharacterStats.Stamina -= Value;
		UE_LOG(LogTemp, Log, TEXT("Yes!!"));
		CanUseStamina = true;		
	}
	else if (CharacterStats.Stamina <= 0)
	{
		CharacterStats.Stamina = 0;
		CanUseStamina = false;
		UE_LOG(LogTemp, Log, TEXT("No!!"));
	}
	return CanUseStamina;
}

void ASoulLikeCharacterBase::CheckStaminaTime(float DeltaTime)
{
	if (NowStaminaChargingTime >= StaminaChargingTime)
	{
		if (CharacterStats.Stamina < CharacterStats.MaxStamina) 
		{
			SetStamina(StaminaChargingValue * DeltaTime);
		}
		else
		{
			CharacterStats.Stamina = CharacterStats.MaxStamina;
		}
	}
	
	else
	{
		NowStaminaChargingTime += 1 * DeltaTime;
	}
}

void ASoulLikeCharacterBase::SetStamina(float Stamina)
{
	CharacterStats.Stamina += Stamina;
	
	if (CharacterStats.Stamina <= 0)
	{
		CharacterStats.Stamina = 0;
	}

	else if (CharacterStats.Stamina > CharacterStats.MaxStamina)
	{
		CharacterStats.Stamina = CharacterStats.MaxStamina;
	}
}

void ASoulLikeCharacterBase::GetDamage(float Damage)
{
	CharacterStats.Health -= Damage;
}

void ASoulLikeCharacterBase::BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ASoulLikeCharacterBase::EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

float ASoulLikeCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	return DamageAmount;
}

void ASoulLikeCharacterBase::HitEvent(FVector HitEvent)
{

}
