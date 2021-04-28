// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBossAnimInstance.h"
#include "EnemyBoss.h"

void UEnemyBossAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();

		if (Pawn)
		{
			EnemyBoss = Cast<AEnemyBoss>(Pawn);

			if (EnemyBoss)
			{
				MoveToTargetSpeed = EnemyBoss->MoveToTargetSpeed;
				DashSpeed = EnemyBoss->DashSpeed;
			}
		}
	}
}

void UEnemyBossAnimInstance::UpdateAniationProprties()
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

		if (EnemyBoss == nullptr)
		{
			EnemyBoss = Cast<AEnemyBoss>(Pawn);
		}
	}
}