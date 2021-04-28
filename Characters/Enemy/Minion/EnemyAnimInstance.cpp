// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "EnemyBase.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();

		if (Pawn)
		{
			Enemy = Cast<AEnemyBase>(Pawn);
		}
	}
}

void UEnemyAnimInstance::UpdateAniationProprties()
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

		if (Enemy == nullptr)
		{
			Enemy = Cast<AEnemyBase>(Pawn);
		}
	}

}