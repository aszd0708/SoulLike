// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerAnimInstance.h"
#include "MainPlayer.h"

void UMainPlayerAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();

		if (Pawn)
		{
			MainPlayer = Cast<AMainPlayer>(Pawn);
		}
	}
}

void UMainPlayerAnimInstance::UpdateAniationProprties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
		MovementSpeed = LateralSpeed.Size();

		MovementYaw = CalculateDirection(Pawn->GetVelocity(), Pawn->GetBaseAimRotation());

		HitDirectionX = MainPlayer->HitDirection.X;
		HitDirectionY = MainPlayer->HitDirection.Y;

		if (MainPlayer == nullptr)
		{
			MainPlayer = Cast<AMainPlayer>(Pawn);
		}
	}
}