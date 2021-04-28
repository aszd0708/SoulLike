// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMinion.h"
#include "AIController.h"
#include "MainPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/Vector.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"

AEnemyMinion::AEnemyMinion()
{
	CharacterStats.Health = 100;
	CharacterStats.MaxHealth = 100;
	CharacterStats.Stamina = 100;
	CharacterStats.MaxStamina = 100;

	FaceOffAngle = 10;
	StartingPoseDistance = 20;

	WeaponCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollider"));
	WeaponCollider->SetupAttachment(GetMesh(), FName("WeaponSocket"));
	WeaponCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	WeaponCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	WeaponCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FAttackInfo Info = { FName("Attack1"), 30.f, 3.f };
	AttackInfos.Emplace(Info);
	Info = { FName("Attack2"), 30.f, 3.f };
	AttackInfos.Emplace(Info);
	Info = { FName("ComboAttack"), 20.f, 5.f };
	AttackInfos.Emplace(Info);
	bIsAttacking = false;
	bOutOfCombatCollision = true;

	NowAttackInfo = AttackInfos[2];

	NowAttackCoolTime = 5;
}

void AEnemyMinion::BeginPlay()
{
	Super::BeginPlay();
	if (AIController)
	{
		AIController = Cast<AAIController>(GetController());
	}
	StartingPose = GetTransform().GetLocation();

	AgroCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMinion::BeginOnOverlapAgroSphere);
	AgroCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemyMinion::EndOnOverlapAgroSphere);

	FaceOffCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMinion::BeginOnOverlapFaceOffSphere);
	FaceOffCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemyMinion::EndOnOverlapFaceOffSphere);

	CombatCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMinion::BeginOnOverlapCombatSphere);
	CombatCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemyMinion::EndOnOverlapCombatSphere);


	WeaponCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMinion::BeginOnOverlapWeapon);
	WeaponCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemyMinion::EndOnOverlapWeapon);
}

void AEnemyMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	EnemyDeltaTime = DeltaTime;
	NowAttackCoolTime += DeltaTime;

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
void AEnemyMinion::IdleEnter()
{
	Super::IdleEnter();

	GetCharacterMovement()->MaxWalkSpeed = MoveToTargetMovmentSpeed;
}
void AEnemyMinion::IdleUpdate()
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
void AEnemyMinion::IdleExit()
{
	Super::IdleExit();

}
#pragma endregion 

#pragma region MoveToTarget
void AEnemyMinion::MoveToTargetEnter(AActor* Target)
{
	Super::MoveToTargetEnter(Target);

	GetCharacterMovement()->MaxWalkSpeed = MoveToTargetMovmentSpeed;
	UE_LOG(LogTemp, Log, TEXT("MoveToTargetEnter!!"));
}

void AEnemyMinion::MoveToTargetUpdate(AActor* Target)
{
	if (bIsAttacking)
	{
		return;
	}

	Super::MoveToTargetUpdate(Target);

	if (Target)
	{
		if (NowAttackCoolTime >= NowAttackInfo.AttackCoolTime)
		{
			GetCharacterMovement()->MaxWalkSpeed = MoveToTargetMovmentSpeed;
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(Target);
			MoveRequest.SetAcceptanceRadius(10.0f);

			FNavPathSharedPtr NavPath;

			AIController->MoveTo(MoveRequest, &NavPath);
			AIController->SetFocus(Target);

			if (FVector::Distance(GetActorLocation(), Target->GetActorLocation()) <= CombatCollider->GetScaledCapsuleRadius() * 1.5f)
			{
				FSMSet(EEnemyMovementStatus::EMS_Attack, Target);
			}
		}

		else
		{

			if (AIController)
			{
				FAIMoveRequest MoveRequest;
				MoveRequest.SetGoalActor(Target);
				MoveRequest.SetAcceptanceRadius(10.0f);

				FNavPathSharedPtr NavPath;

				AIController->MoveTo(MoveRequest, &NavPath);
				AIController->SetFocus(Target);
			}
		}
	}
	
}

void AEnemyMinion::MoveToTargetExit()
{
	Super::MoveToTargetExit();

}
#pragma endregion 

#pragma region FaceOff
void AEnemyMinion::FaceOffEnter(AActor* Target)
{
	Super::FaceOffEnter(Target);

	GetCharacterMovement()->MaxWalkSpeed = FaceOffMovementSpeed;

	if (Target)
	{
		FaceOffAngle = FMath::Acos(FVector::DotProduct(GetActorLocation(), Target->GetActorLocation()));
	}
	UE_LOG(LogTemp, Log, TEXT("FaceOffEnter!!"));
}
	
void AEnemyMinion::FaceOffUpdate(AActor* Target)
{
	if (bIsAttacking)
	{
		return;
	}
	Super::FaceOffUpdate(Target);

	if (AIController)
	{
		if (Target)
		{
			if (NowAttackCoolTime >= NowAttackInfo.AttackCoolTime)
			{
				GetCharacterMovement()->MaxWalkSpeed = MoveToTargetMovmentSpeed;
				FAIMoveRequest MoveRequest;
				MoveRequest.SetGoalActor(Target);
				MoveRequest.SetAcceptanceRadius(10.0f);

				FNavPathSharedPtr NavPath;

				AIController->MoveTo(MoveRequest, &NavPath);
				AIController->SetFocus(Target);

				if (FVector::Distance(GetActorLocation(), Target->GetActorLocation()) <= CombatCollider->GetScaledCapsuleRadius() * 1.5f)
				{
					FSMSet(EEnemyMovementStatus::EMS_Attack, Target);
				}
			}
			else
			{
				FAIMoveRequest MoveRequest;
				MoveRequest.SetAcceptanceRadius(1.0f);
				FNavPathSharedPtr NavPath;

				float Distance = FaceOffCollider->GetScaledSphereRadius() - 50;
				if (Distance > FaceOffCollider->GetScaledSphereRadius())
				{
					Distance = FaceOffCollider->GetScaledSphereRadius();
				}

				FaceOffAngle += 1 * EnemyDeltaTime;
				if (FaceOffAngle >= 360)
				{
					FaceOffAngle = 0;
				}

				//FVector UnitLocation = GetActorLocation();
				//FVector DirectionToActor = (Target->GetActorLocation() - UnitLocation).GetSafeNormal();
				//
				//DirectionToActor.Z = 0;

				//FVector Perpendicular = DirectionToActor.RotateAngleAxis(FaceOffAngle, FVector(0, 0, 1))*Distance;
				float SinX = FMath::Cos(FaceOffAngle) * Distance;
				float CosY = FMath::Sin(FaceOffAngle) * Distance;


				//UE_LOG(LogTemp, Log, TEXT("X : %f"), Perpendicular.X);
				//UE_LOG(LogTemp, Log, TEXT("Y : %f"), Perpendicular.Y);		

				//FVector Destination = FVector(GetActorLocation().X + Perpendicular.X, GetActorLocation().Y + Perpendicular.Y, GetActorLocation().Z);
				FVector Destination = FVector(Target->GetActorLocation().X + SinX, Target->GetActorLocation().Y + CosY, GetActorLocation().Z);
				MoveRequest.SetGoalLocation(Destination);
				//AIController->SetFocus(Target);
				AIController->MoveTo(MoveRequest, &NavPath);
				//UE_LOG(LogTemp, Log, TEXT(" X : %2f"), Destination.X);
				//UE_LOG(LogTemp, Log, TEXT(" Y : %2f"), Destination.Y);
				//UKismetSystemLibrary::DrawDebugSphere(this, Destination, 10.f, 25.f, FLinearColor::Red, 10.f, 1.f);
			}

		}

	}
}
void AEnemyMinion::FaceOffExit()
{
	Super::FaceOffExit();

}
#pragma endregion 

#pragma region Attack
void AEnemyMinion::AttackEnter(AActor* Target)
{
	Super::AttackEnter(Target);
	//if (!bIsAttacking && NowAttackCoolTime >= NowAttackInfo.AttackCoolTime)
	//{
	//	NowAttackInfo = GetRandomAttackInfo();
	//
	//	PlayAttackMontage();
	//}
}
void AEnemyMinion::AttackUpdate(AActor* Target)
{
	Super::AttackUpdate(Target);

	if (!bIsAttacking && NowAttackCoolTime >= NowAttackInfo.AttackCoolTime)
	{
		NowAttackInfo = GetRandomAttackInfo();
		
		PlayAttackMontage();
	}
}
void AEnemyMinion::AttackExit()
{
	Super::AttackExit();
}
#pragma endregion 

#pragma region Hit
void AEnemyMinion::HitEnter()
{
	Super::HitEnter();


}
void AEnemyMinion::HitUpdate()
{
	Super::HitUpdate();

}
void AEnemyMinion::HitExit()
{
	Super::HitExit();

}

#pragma region Dead
void AEnemyMinion::DeadEnter()
{
	Super::DeadEnter();

	bIsDead = true; 

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
void AEnemyMinion::DeadUpdate()
{
	Super::DeadUpdate();

}
void AEnemyMinion::DeadExit()
{
	Super::DeadExit();

}
#pragma endregion 

#pragma endregion 

#pragma region Collisions
void AEnemyMinion::BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOnOverlapWeapon(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
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
		UE_LOG(LogTemp, Log, TEXT("Actor! %s"), *OtherActor->GetFullName());
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{

			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(MainPlayer, NowAttackInfo.AttackDamage, AIController, this, DamageTypeClass);
			}

			const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
			if (TipSocket)
			{
				FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
				FVector Location = MainPlayer->GetMesh()->GetClosestCollidingRigidBodyLocation(SocketLocation);
				MainPlayer->HitEvent(Location);
			}
			UE_LOG(LogTemp, Log, TEXT("Main!!"));
		}
	}
}

void AEnemyMinion::EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOnOverlapWeapon(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AEnemyMinion::BeginOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
			FSMSet(EEnemyMovementStatus::EMS_MoveToTarget, OtherActor);
		}
	}
}

void AEnemyMinion::EndOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void AEnemyMinion::BeginOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOnOverlapCombatSphere(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (bIsDead)
	{
		return;
	}
	if (MovementStatus == EEnemyMovementStatus::EMS_Hit)
	{
		return;
	}

	if (!bIsAttacking)
	{
		if (OtherActor)
		{
			AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);

			if (MainPlayer)
			{
				FSMSet(EEnemyMovementStatus::EMS_Attack, OtherActor);
				UE_LOG(LogTemp, Log, TEXT("InsideCombatCollider"));
			}
		}
	}
	bOutOfCombatCollision = false;
}

void AEnemyMinion::EndOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOnOverlapCombatSphere(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
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
			FSMSet(EEnemyMovementStatus::EMS_FaceOff, OtherActor);
			UE_LOG(LogTemp, Log, TEXT("InsideCombatCollider"));
		}
	}
	bOutOfCombatCollision = true;
}

void AEnemyMinion::BeginOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
			FSMSet(EEnemyMovementStatus::EMS_FaceOff, OtherActor);
		}
	}
}

void AEnemyMinion::EndOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

	if (bIsAttacking)
	{
		if (OtherActor)
		{
			AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);

			if (MainPlayer)
			{
				FSMSet(EEnemyMovementStatus::EMS_MoveToTarget, OtherActor);
			}
		}
	}
}

#pragma endregion 

#pragma region Attack

void AEnemyMinion::StartAttack()
{
	SetCollision(true);
	SetIsAttack(true);
	if (AIController)
	{
		AIController->SetFocus(nullptr);
	}


}

void AEnemyMinion::EndAttack()
{
	SetCollision(false);
	NowAttackCoolTime = 0;
	FSMSet(EEnemyMovementStatus::EMS_FaceOff, FSMTarget);
	if (AIController)
	{
		if (FSMTarget)
		{
			AIController->SetFocus(FSMTarget);
		}
	}
}

void AEnemyMinion::SetCollision(bool IsOn)
{
	if (IsOn)
	{
		WeaponCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (SwingSound)
		{
			UGameplayStatics::PlaySound2D(this, SwingSound);
		}
	}

	else
	{
		WeaponCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AEnemyMinion::SetAttackOtherWeapon(bool IsOn)
{

}

FAttackInfo AEnemyMinion::GetRandomAttackInfo()
{
	int RandomIndex = FMath::RandRange(0, AttackInfos.Num() - 1);
	return AttackInfos[RandomIndex];
}

void AEnemyMinion::SetIsAttack(bool Attack)
{
	bIsAttacking = Attack;
}

void AEnemyMinion::PlayAttackMontage()
{
	if (CombatMontage)
	{
		// АјАн

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (AIController)
			{
				AIController->StopMovement();
			}
			UE_LOG(LogTemp, Log, TEXT("EnemyAttack"));
			AnimInstance->Montage_Play(CombatMontage, 0.8f);
			AnimInstance->Montage_JumpToSection(NowAttackInfo.AttackName);
			bIsAttacking = true;
		}
	}
	else
	{

		UE_LOG(LogTemp, Log, TEXT("NonMontage Enemy Attack"));
	}
}

#pragma endregion

float AEnemyMinion::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
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

void AEnemyMinion::HitEvent(FVector HitPose)
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

	if (bIsAttacking)
	{
		bIsAttacking = false;
	}
}

void AEnemyMinion::StartHitEvent()
{
	//if (GetMovementStatus() != EEnemyMovementStatus::EMS_Dead)
	//{
	//	FSMSet(EEnemyMovementStatus::EMS_Hit);
	//}
}

void AEnemyMinion::EndHitEvent()
{
	//if (GetMovementStatus() != EEnemyMovementStatus::EMS_Dead)
	//{
	//	FSMSet(PrevMovementStatus);
	//}
}

void AEnemyMinion::EndDeathEvent()
{
	Super::EndDeathEvent();


}