// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMinionGun.h"
#include "AIController.h"
#include "Bullet.h"
#include "MainPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"

AEnemyMinionGun::AEnemyMinionGun()
{
	BulletDamage = 30.f;
	ShootCooltime = 5.f;
	NowShootCooltime = 0.f;

	StartingPoseDistance = 10.f;

	bIsAttacking = false;
}

void AEnemyMinionGun::BeginPlay()
{
	Super::BeginPlay();

	if (AIController)
	{
		AIController = Cast<AAIController>(GetController());
	}

	StartingPose = GetActorLocation();

	AgroCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMinionGun::BeginOnOverlapAgroSphere);
	AgroCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemyMinionGun::EndOnOverlapAgroSphere);

	FaceOffCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMinionGun::BeginOnOverlapFaceOffSphere);
	FaceOffCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemyMinionGun::EndOnOverlapFaceOffSphere);

	CombatCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMinionGun::BeginOnOverlapCombatSphere);
	CombatCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemyMinionGun::EndOnOverlapCombatSphere);
}

void AEnemyMinionGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	NowShootCooltime += DeltaTime; 

	if (bOutOfCombatCollision)
	{
		if (!bIsAttacking)
		{
			if (FSMTarget)
			{
				FSMSet(EEnemyMovementStatus::EMS_MoveToTarget, FSMTarget);
				bOutOfCombatCollision = false;
			}
		}
	}
}


#pragma region FSM

#pragma region Idle
void AEnemyMinionGun::IdleEnter()
{
	Super::IdleEnter();
	if (AIController)
	{
		AIController->SetFocus(nullptr);
	}

	GetCharacterMovement()->MaxWalkSpeed = MoveToTargetMovmentSpeed;
}
void AEnemyMinionGun::IdleUpdate()
{
	Super::IdleUpdate();

	if (AIController)
	{
		float NowStartingPoseDistance = (GetTransform().GetLocation() - StartingPose).Size();

		if (NowStartingPoseDistance <= StartingPoseDistance)
		{
			AIController->StopMovement();
		}

		else
		{
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalLocation(StartingPose);
			MoveRequest.SetAcceptanceRadius(10.0f);

			FNavPathSharedPtr NavPath;

			AIController->MoveTo(MoveRequest, &NavPath);
		}
	}
}
void AEnemyMinionGun::IdleExit()
{

}
#pragma endregion 

#pragma region MoveToTarget
void AEnemyMinionGun::MoveToTargetEnter(AActor* Target)
{
	Super::MoveToTargetEnter(Target);

	GetCharacterMovement()->MaxWalkSpeed = MoveToTargetMovmentSpeed;
	UE_LOG(LogTemp, Log, TEXT("MoveToTargetEnter!!"));
}
void AEnemyMinionGun::MoveToTargetUpdate(AActor* Target)
{
	if (bIsAttacking)
	{
		return;
	}

	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f);

		FNavPathSharedPtr NavPath;

		AIController->SetFocus(Target);
		AIController->MoveTo(MoveRequest, &NavPath);

		if (FVector::Distance(GetActorLocation(), Target->GetActorLocation()) <= FaceOffCollider->GetScaledSphereRadius() * 1.1f)
		{
			FSMSet(EEnemyMovementStatus::EMS_FaceOff, Target);
		}
	}
}
void AEnemyMinionGun::MoveToTargetExit()
{

}

#pragma endregion 

#pragma region FaceOff
void AEnemyMinionGun::FaceOffEnter(AActor* Target)
{
	if (AIController)
	{
		AIController->StopMovement();
		if (Target)
		{
			AIController->SetFocus(Target);
		}
	}
}
void AEnemyMinionGun::FaceOffUpdate(AActor* Target)
{

	if (NowShootCooltime >= ShootCooltime)
	{
		// 공격if (Target)
		{
			FSMSet(EEnemyMovementStatus::EMS_Attack, Target);
			NowShootCooltime = 0;
		}
	}
}
void AEnemyMinionGun::FaceOffExit()
{
	if (AIController)
	{
		AIController->SetFocus(nullptr);
	}
}
#pragma endregion 

#pragma region Attack
void AEnemyMinionGun::AttackEnter(AActor* Target)
{
	if (AIController)
	{
		if (Target)
		{
			AIController->SetFocus(Target);
		}
	}

	if (CombatMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.0f);
		}
	}
}
void AEnemyMinionGun::AttackUpdate(AActor* Target)
{
	if (bIsDead)
	{
		return;
	}

	if (NowShootCooltime >= ShootCooltime)
	{
		// 공격
		if (Target)
		{
			FSMSet(EEnemyMovementStatus::EMS_Attack, Target);
			NowShootCooltime = 0;
		}
	}
}
void AEnemyMinionGun::AttackExit()
{

}
#pragma endregion 

#pragma region Hit
void AEnemyMinionGun::HitEnter()
{
	if (HitMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance)
		{
			AnimInstance->Montage_Play(HitMontage);
		}
	}
}
void AEnemyMinionGun::HitUpdate()
{

}
void AEnemyMinionGun::HitExit()
{

}
#pragma endregion 


#pragma region Dead
void AEnemyMinionGun::DeadEnter()
{
	Super::DeadEnter();

	if (AIController)
	{
		AIController->StopMovement();
		AIController->SetFocus(nullptr);
	}

	//if (DeathMontage)
	//{
	//	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	//
	//	AnimInstance->StopAllMontages(true);
	//	AnimInstance->StopSlotAnimation();
	//
	//	FName DeathName;
	//	int RandomIndex = FMath::RandRange(0, 2);
	//	switch (RandomIndex)
	//	{
	//	case 0:
	//		DeathName = FName("Death_1");
	//		break;
	//	case 1:
	//		DeathName = FName("Death_2");
	//		break;
	//	case 2:
	//		DeathName = FName("Death_3");
	//		break;
	//	case 3:
	//		DeathName = FName("Death_4");
	//		break;
	//	default:
	//		DeathName = FName("Death_1");
	//		break;
	//	}
	//	AnimInstance->Montage_Play(DeathMontage, 1.0f);
	//	AnimInstance->Montage_JumpToSection(DeathName);
	//}
}
void AEnemyMinionGun::DeadUpdate()
{

}
void AEnemyMinionGun::DeadExit()
{

}
#pragma endregion 

#pragma endregion 

#pragma region Collisions
void AEnemyMinionGun::BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOnOverlapWeapon(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AEnemyMinionGun::EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOnOverlapWeapon(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}

void AEnemyMinionGun::BeginOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOnOverlapAgroSphere(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (bIsDead)
	{
		return;
	}
	if (MovementStatus == EEnemyMovementStatus::EMS_Hit)
	{
		return;
	}

	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			FSMSet(EEnemyMovementStatus::EMS_MoveToTarget, MainPlayer);
		}
	}
}

void AEnemyMinionGun::EndOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOnOverlapAgroSphere(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (bIsDead)
	{
		return;
	}
	if (MovementStatus == EEnemyMovementStatus::EMS_Hit)
	{
		return;
	}

	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			FSMSet(EEnemyMovementStatus::EMS_Idle);
		}
	}
}

void AEnemyMinionGun::BeginOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOnOverlapCombatSphere(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

void AEnemyMinionGun::EndOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOnOverlapCombatSphere(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}

void AEnemyMinionGun::BeginOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOnOverlapFaceOffSphere(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (bIsDead)
	{
		return;
	}
	if (MovementStatus == EEnemyMovementStatus::EMS_Hit)
	{
		return;
	}

	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);

		if (MainPlayer)
		{
			FSMTarget = MainPlayer;

			FSMSet(EEnemyMovementStatus::EMS_FaceOff, MainPlayer);
		}
	}
	bOutOfCombatCollision = false;	
}

void AEnemyMinionGun::EndOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOnOverlapFaceOffSphere(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (bIsDead)
	{
		return;
	}
	if (MovementStatus == EEnemyMovementStatus::EMS_Hit)
	{
		return;
	}

	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			FSMSet(EEnemyMovementStatus::EMS_MoveToTarget, MainPlayer);
		}
	}
	bOutOfCombatCollision = true;
}

#pragma endregion

void AEnemyMinionGun::StartAttack()
{
	if (AIController)
	{	
		AIController->SetFocus(nullptr);
	}
	FVector GunFirePose = GetMesh()->GetSocketLocation(FName("MuzzleSocket"));
	
	SetIsAttack(true);
	if (Bullet)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParameter;

		if (World)
		{
			AActor* BulletActor = World->SpawnActor<AActor>(Bullet, GunFirePose, GetActorRotation(), SpawnParameter);

			if (BulletActor)
			{
				ABullet* BulletInfo = Cast<ABullet>(BulletActor);
				BulletInfo->SetShootActor(this);
				BulletInfo->SetDamage(BulletDamage);
				BulletInfo->FireInDirection(GunFirePose);
			}
		}

		if (ShootParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShootParticle, GunFirePose);
		}

		if (ShootSound)
		{
			UGameplayStatics::PlaySound2D(this, ShootSound);
		}
	}
}

void AEnemyMinionGun::EndAttack()
{
	SetIsAttack(false);

	FSMSet(EEnemyMovementStatus::EMS_FaceOff, FSMTarget);
}

float AEnemyMinionGun::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return DamageAmount;
	}
	CharacterStats.Health -= DamageAmount;
	if (CharacterStats.Health <= 0)
	{
		FSMSet(EEnemyMovementStatus::EMS_Dead);
	}
	return DamageAmount;
}

void AEnemyMinionGun::HitEvent(FVector HitPose)
{
	if (bIsDead)
	{
		return;
	}

	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitPose);
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HitSound);
	}

	if (HitMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (HitPose.X > 0)
		{
			AnimInstance->Montage_Play(HitMontage, 1.0f);
			AnimInstance->Montage_JumpToSection("RightHit");
		}
		else
		{
			AnimInstance->Montage_Play(HitMontage, 1.0f);
			AnimInstance->Montage_JumpToSection("LeftHit");
		}
	}
}

void AEnemyMinionGun::StartHitEvent()
{
	//if (GetMovementStatus() != EEnemyMovementStatus::EMS_Dead)
	//{
	//	FSMSet(EEnemyMovementStatus::EMS_Hit);
	//}
}

void AEnemyMinionGun::EndHitEvent()
{
	//if (GetMovementStatus() != EEnemyMovementStatus::EMS_Dead)
	//{
	//	FSMSet(PrevMovementStatus);
	//}
}

void AEnemyMinionGun::EndDeathEvent()
{
	Super::EndDeathEvent();

}

void AEnemyMinionGun::SetIsAttack(bool Attack)
{
	bIsAttacking = Attack;
}