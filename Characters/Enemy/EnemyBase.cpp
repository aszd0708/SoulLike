// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Sound/SoundWave.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyBase::AEnemyBase()
{
	CombatCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CabatCollider"));
	CombatCollider->SetupAttachment(GetRootComponent()); 

	AgroCollider = CreateDefaultSubobject<USphereComponent>(TEXT("AgroCollider"));
	AgroCollider->SetupAttachment(GetRootComponent());

	FaceOffCollider = CreateDefaultSubobject<USphereComponent>(TEXT("FaceOffCollider"));
	FaceOffCollider->SetupAttachment(GetRootComponent());

	MovementStatus = EEnemyMovementStatus::EMS_Idle;

	MoveToTargetMovmentSpeed = 450.f;
	FaceOffMovementSpeed = 100.f;
	FSMStatus = EFSMStatus::EFS_Enter;

	bIsDead = false;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FSM(FSMTarget);
}
#pragma region FSM


void AEnemyBase::FSM(AActor* Target)
{
	switch (FSMStatus)
	{
	case EFSMStatus::EFS_Enter:
		FSMEnter(Target);
	case EFSMStatus::EFS_Update:
		FSMUpdate(Target);
	case EFSMStatus::EFS_Exit:
		return;
	default:
		return;
	}
}

void AEnemyBase::FSMSet(EEnemyMovementStatus Status, AActor* Target)
{
	PrevMovementStatus = MovementStatus;
	FSMStatus = EFSMStatus::EFS_Exit;
	FSMExit(MovementStatus);
	MovementStatus = Status;
	FSMStatus = EFSMStatus::EFS_Enter;
	FSMTarget = Target;
}

void AEnemyBase::FSMEnter(AActor* Target)
{
	switch (MovementStatus)
	{
	case EEnemyMovementStatus::EMS_Idle:
		IdleEnter();
		break;

	case EEnemyMovementStatus::EMS_MoveToTarget:
		MoveToTargetEnter(Target);
		break;

	case EEnemyMovementStatus::EMS_FaceOff:
		FaceOffEnter(Target);
		break;

	case EEnemyMovementStatus::EMS_Attack:
		AttackEnter(Target);
		break;

	case EEnemyMovementStatus::EMS_Hit:
		HitEnter();
		break;

	case EEnemyMovementStatus::EMS_Dead:
		DeadEnter();
		break;

	default:
		return;
	}
	FSMStatus = EFSMStatus::EFS_Update;
}

void AEnemyBase::FSMUpdate(AActor* Target)
{
	switch (MovementStatus)
	{
	case EEnemyMovementStatus::EMS_Idle:
		IdleUpdate();
		break;

	case EEnemyMovementStatus::EMS_MoveToTarget:
		MoveToTargetUpdate(Target);
		break;

	case EEnemyMovementStatus::EMS_FaceOff:
		FaceOffUpdate(Target);
		break;

	case EEnemyMovementStatus::EMS_Attack:
		AttackUpdate(Target);
		break;

	case EEnemyMovementStatus::EMS_Hit:
		HitUpdate();
		break;

	case EEnemyMovementStatus::EMS_Dead:
		DeadUpdate();
		break;

	default:
		return;
	}
}

void AEnemyBase::FSMExit(EEnemyMovementStatus Status)
{
	switch (Status)
	{
	case EEnemyMovementStatus::EMS_Idle:
		IdleExit();
		break;

	case EEnemyMovementStatus::EMS_MoveToTarget:
		MoveToTargetExit();
		break;

	case EEnemyMovementStatus::EMS_FaceOff:
		FaceOffExit();
		break;

	case EEnemyMovementStatus::EMS_Attack:
		AttackExit();
		break;

	case EEnemyMovementStatus::EMS_Hit:
		HitExit();
		break;

	case EEnemyMovementStatus::EMS_Dead:
		DeadExit();
		break;

	default:
		return;
	}
}

#pragma region Idle
void AEnemyBase::IdleEnter()
{

}
void AEnemyBase::IdleUpdate()
{

}
void AEnemyBase::IdleExit()
{

}
#pragma endregion 

#pragma region MoveToTarget
void AEnemyBase::MoveToTargetEnter(AActor* Target)
{

}
void AEnemyBase::MoveToTargetUpdate(AActor* Target)
{

}
void AEnemyBase::MoveToTargetExit()
{

}
#pragma endregion 

#pragma region FaceOff
void AEnemyBase::FaceOffEnter(AActor* Target)
{

}
void AEnemyBase::FaceOffUpdate(AActor* Target)
{

}
void AEnemyBase::FaceOffExit()
{

}
#pragma endregion 

#pragma region Attack
void AEnemyBase::AttackEnter(AActor* Target)
{

}
void AEnemyBase::AttackUpdate(AActor* Target)
{

}
void AEnemyBase::AttackExit()
{

}
#pragma endregion 

#pragma region Hit
void AEnemyBase::HitEnter()
{				
				
}				
void AEnemyBase::HitUpdate()
{			
			
}			
void AEnemyBase::HitExit()
{

}
#pragma endregion 


#pragma region Dead
void AEnemyBase::DeadEnter()
{
	EndDeathEvent();
}
void AEnemyBase::DeadUpdate()
{

}
void AEnemyBase::DeadExit()
{

}
#pragma endregion 
#pragma endregion 
 

#pragma region Collisions

void AEnemyBase::BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	Super::BeginOnOverlapWeapon(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

void AEnemyBase::EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOnOverlapWeapon(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}

void AEnemyBase::BeginOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AEnemyBase::EndOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemyBase::BeginOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AEnemyBase::EndOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemyBase::BeginOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AEnemyBase::EndOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

#pragma endregion

#pragma region Combat
void AEnemyBase::StartAttack() 
{

}

void AEnemyBase::EndAttack()
{

}

void AEnemyBase::SetCollision(bool IsOn)
{

}

void AEnemyBase::SetIsAttack(bool Attack)
{

}

float AEnemyBase::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	return DamageAmount;
}

void AEnemyBase::HitEvent(FVector HitPose)
{

}
#pragma endregion

void AEnemyBase::StartHitEvent()
{

}

void AEnemyBase::EndHitEvent()
{

}

void AEnemyBase::EndDeathEvent()
{
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);// Ragdoll
	//GetMesh()->bNoSkeletonUpdate = true;
	//GetMesh()->bPauseAnims = true;
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}

	SetLifeSpan(10.0f);
}
void AEnemyBase::SetAttackOtherWeapon(bool IsOn)
{

}

void AEnemyBase::SetDualAttackCollision(bool IsOn)
{
	SetAttackOtherWeapon(IsOn);
	SetCollision(IsOn);
}