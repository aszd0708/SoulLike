// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMinionDualBlade.h"
#include "AIController.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"
#include "MainPlayer.h"

AEnemyMinionDualBlade::AEnemyMinionDualBlade()
{
	OtherWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OtherWeaponCollision"));
	OtherWeaponCollision->SetupAttachment(GetMesh(), FName("OtherWeaponSocket"));
	OtherWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	OtherWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OtherWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	OtherWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyMinionDualBlade::BeginPlay()
{
	Super::BeginPlay();

	OtherWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMinionDualBlade::BeginOnOverlapWeapon);
	OtherWeaponCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemyMinionDualBlade::EndOnOverlapWeapon);
}

void AEnemyMinionDualBlade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyMinionDualBlade::StartAttack()
{
	SetIsAttack(true);
	if (AIController)
	{
		AIController->SetFocus(nullptr);
	}
}

void AEnemyMinionDualBlade::EndAttack()
{
	SetIsAttack(false);
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

void AEnemyMinionDualBlade::SetAttackOtherWeapon(bool IsOn)
{
	if (IsOn)
	{
		OtherWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (SwingSound)
		{
			UGameplayStatics::PlaySound2D(this, SwingSound);
		}
	}
	else
	{
		OtherWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AEnemyMinionDualBlade::BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(MainPlayer, NowAttackInfo.AttackDamage, AIController, this, DamageTypeClass);
			}

			const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket_L");
			if (TipSocket)
			{
				FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
				FVector Location = MainPlayer->GetMesh()->GetClosestCollidingRigidBodyLocation(SocketLocation);
				MainPlayer->HitEvent(Location);
			}
		}
	}
}

void AEnemyMinionDualBlade::EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOnOverlapWeapon(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}