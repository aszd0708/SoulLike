#include "EnemyBoss.h"
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
#include "Kismet/KismetSystemLibrary.h"
#include "DeathBall.h"
#include "Bomb.h"

AEnemyBoss::AEnemyBoss()
{
	MovementStatus = EEnemyMovementStatus::EMS_MAX;
	NowAttackDistance = EDistanceInfomation::EDI_Range;
	AfterAttackChangeStatus = EBossMovementStatus::EMS_MoveToTarget;
	bIsAttack = false;
	WeaponCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollider"));
	WeaponCollider->SetupAttachment(GetMesh(), FName("WeaponSocket"));
	WeaponCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	WeaponCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	WeaponCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EarthQukeExplosionRadius = 300;

	SetAttackInfo();
	bIsDash = false;
	DashSpeed = 500;
}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	if (AIController)
	{
		AIController = Cast<AAIController>(GetController());
	}

	AgroCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBoss::BeginOnOverlapAgroSphere);
	AgroCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemyBoss::EndOnOverlapAgroSphere);

	CombatCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBoss::BeginOnOverlapCombatSphere);
	CombatCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemyBoss::EndOnOverlapCombatSphere);

	FaceOffCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBoss::BeginOnOverlapFaceOffSphere);
	FaceOffCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemyBoss::EndOnOverlapFaceOffSphere);

	WeaponCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBoss::BeginOnOverlapWeapon);
	WeaponCollider->OnComponentEndOverlap.AddDynamic(this, &AEnemyBoss::EndOnOverlapWeapon);
}

void AEnemyBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	NowRecoverTime += DeltaTime;
	NowDashCoolTime += DeltaTime;
	SetBossAttackNowCoolTimes(DeltaTime);

	FSM_Boss(NowFSMTarget);
}

#pragma region FSM
void AEnemyBoss::FSM_Boss(AActor* Target)
{
	switch (FSMStatus)
	{
	case EFSMStatus::EFS_Enter:
		FSMEnter_Boss(Target);
	case EFSMStatus::EFS_Update:
		FSMUpdate_Boss(Target);
	case EFSMStatus::EFS_Exit:
		return;
	default:
		return;
	}
}

void AEnemyBoss::FSMEnter_Boss(AActor* Target)
{
	switch (NowMovementStatus)
	{
	case EBossMovementStatus::EMS_Idle:
		IdleEnter_Boss();
		break;
	case EBossMovementStatus::EMS_MoveToTarget:
		MoveToTargetEnter_Boss(Target);
		break;
	case EBossMovementStatus::EMS_FaceOff:
		FaceOffEnter_Boss(Target);
		break;
	case EBossMovementStatus::EMS_Attack:
		AttackEnter_Boss(Target);
		break;
	case EBossMovementStatus::EMS_Dash:
		DashEnter_Boss(Target);
		break;
	default:
		break;
	}
	FSMStatus = EFSMStatus::EFS_Update;
}

void AEnemyBoss::FSMUpdate_Boss(AActor* Target)
{
	switch (NowMovementStatus)
	{
	case EBossMovementStatus::EMS_Idle:
		IdleUpdate_Boss();
		break;
	case EBossMovementStatus::EMS_MoveToTarget:
		MoveToTargetUpdate_Boss(Target);
		break;
	case EBossMovementStatus::EMS_FaceOff:
		FaceOffUpdate_Boss(Target);
		break;
	case EBossMovementStatus::EMS_Attack:
		AttackUpdate_Boss(Target);
		break;
	case EBossMovementStatus::EMS_Dash:
		DashUpdate_Boss(Target);
		break;
	default:
		break;
	}
}

void AEnemyBoss::FSMExit_Boss(EBossMovementStatus Status)
{
	switch (NowMovementStatus)
	{
	case EBossMovementStatus::EMS_Idle:
		IdleExit_Boss();
		break;
	case EBossMovementStatus::EMS_MoveToTarget:
		MoveToTargetExit_Boss();
		break;
	case EBossMovementStatus::EMS_FaceOff:
		FaceOffExit_Boss();
		break;
	case EBossMovementStatus::EMS_Attack:
		AttackExit_Boss();
		break;
	case EBossMovementStatus::EMS_Dash:
		DashExit_Boss();
		break;
	default:
		break;
	}
}

void AEnemyBoss::FSMSet_Boss(EBossMovementStatus Status, AActor* Target)
{
	//FSMTarget = Target;
	NowFSMTarget = Target;
	PrevMovementStatus = NowMovementStatus;
	FSMStatus = EFSMStatus::EFS_Exit;
	FSMExit_Boss(NowMovementStatus);
	NowMovementStatus = Status;
	FSMStatus = EFSMStatus::EFS_Enter;
}

#pragma region Idle
void AEnemyBoss::IdleEnter_Boss()
{

}

void AEnemyBoss::IdleUpdate_Boss()
{

}

void AEnemyBoss::IdleExit_Boss()
{

}
#pragma endregion 

#pragma region MoveToTarget
void AEnemyBoss::MoveToTargetEnter_Boss(AActor* Target)
{
	GetCharacterMovement()->MaxWalkSpeed = MoveToTargetSpeed;
}

void AEnemyBoss::MoveToTargetUpdate_Boss(AActor* Target)
{
	if (Target)
	{
		if (DashCoolTime <= NowDashCoolTime)
		{
			FSMSet_Boss(EBossMovementStatus::EMS_Dash, Target);
		}

		else if (NowAttack->RecoverTime <= NowRecoverTime)
		{
			FSMSet_Boss(EBossMovementStatus::EMS_Attack, Target);
		}

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

void AEnemyBoss::MoveToTargetExit_Boss()
{

}
#pragma endregion 

#pragma region FaceOff
void AEnemyBoss::FaceOffEnter_Boss(AActor* Target)
{
	GetCharacterMovement()->MaxWalkSpeed = FaceOffSpeed;
	AIController->SetFocus(Target);
}

void AEnemyBoss::FaceOffUpdate_Boss(AActor* Target)
{
	if (Target)
	{
		if (NowAttack->RecoverTime <= NowRecoverTime)
		{
			FSMSet_Boss(EBossMovementStatus::EMS_Attack, Target);
		}

		else
		{
			if (AIController)
			{
				// 타겟으로 가는 
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

void AEnemyBoss::FaceOffExit_Boss()
{

}
#pragma endregion 

void AEnemyBoss::AttackEnter_Boss(AActor* Target)
{
	bIsAttack = true;
	int32 Count = 0;

	AIController->SetFocus(nullptr);
	AIController->StopMovement();
	

	while (&NowAttack)
	{
		//NowAttack = GetAttackInfo();
		UE_LOG(LogTemp, Log, TEXT("Select Attack"));
		SetAttackInfo();
		Count++;
		if (Count > 10)
		{
			break;
		}
	}
	UE_LOG(LogTemp, Log, TEXT("%s"), NowAttack->AttackMontage);
	if (!&NowAttack|| NowAttack == &Attack_Empty)
	{
		if (Target)
		{
			NowRecoverTime = 0;
			FSMSet_Boss(EBossMovementStatus::EMS_FaceOff, Target);
		}
	}

	else
	{
		PlayAttackMontage();
	}
}

void AEnemyBoss::AttackUpdate_Boss(AActor* Target)
{
	AIController->StopMovement();
}

void AEnemyBoss::AttackExit_Boss()
{

}

#pragma region Dash
void AEnemyBoss::DashEnter_Boss(AActor* Target)
{
	GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
	bIsDash = true;
}

void AEnemyBoss::DashUpdate_Boss(AActor* Target)
{
	if (Target)
	{
		if (AIController)
		{
			// 타겟으로 가는 
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(Target);
			MoveRequest.SetAcceptanceRadius(10.0f);

			FNavPathSharedPtr NavPath;

			AIController->MoveTo(MoveRequest, &NavPath);
			AIController->SetFocus(Target);

			float Distance = FVector::Distance(Target->GetActorLocation(), GetActorLocation());
			if (Distance <= DashDistance)
			{
				FSMSet_Boss(EBossMovementStatus::EMS_FaceOff, Target);
			}
		}
	}
}

void AEnemyBoss::DashExit_Boss()
{
	bIsDash = false;
}
#pragma endregion 

#pragma endregion 

#pragma region Collisions
void AEnemyBoss::BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);

		if (MainPlayer)
		{
			UE_LOG(LogTemp, Log, TEXT("MainPlayer"));
			UGameplayStatics::ApplyDamage(MainPlayer, NowAttack->AttackDamage, NULL, this, DamageTypeClass);

			USkeletalMeshComponent* HitCharacterMesh = MainPlayer->GetMesh();
			if (HitCharacterMesh)
			{
				FVector HitLocation = HitCharacterMesh->GetClosestCollidingRigidBodyLocation(GetActorLocation());
				MainPlayer->HitEvent(HitLocation);
			}
		}
	}
}

void AEnemyBoss::EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemyBoss::BeginOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			SetNowAttackDistance(EDistanceInfomation::EDI_Range);
			UE_LOG(LogTemp, Log, TEXT("Enter Agro Sphere"));
			if (bIsAttack)
			{
				AfterAttackChangeStatus = EBossMovementStatus::EMS_MoveToTarget;
			}
			else
			{
				if (!bIsDash)
				{
					FSMSet_Boss(EBossMovementStatus::EMS_MoveToTarget, MainPlayer);
				}
			}
		}
	}
}

void AEnemyBoss::EndOnOverlapAgroSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			SetNowAttackDistance(EDistanceInfomation::EDI_Range);
			//FSMSet_Boss(EBossMovementStatus::EMS_Idle);
		}
	}
}

void AEnemyBoss::BeginOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			SetNowAttackDistance(EDistanceInfomation::EDI_Middle);

			if (bIsAttack)
			{
				AfterAttackChangeStatus = EBossMovementStatus::EMS_FaceOff;
			}
			if (!bIsDash)
			{
				FSMSet_Boss(EBossMovementStatus::EMS_FaceOff, MainPlayer);
			}
		}
	}
}

void AEnemyBoss::EndOnOverlapFaceOffSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			SetNowAttackDistance(EDistanceInfomation::EDI_Range);
			if (bIsAttack)
			{
				AfterAttackChangeStatus = EBossMovementStatus::EMS_MoveToTarget;
			}
			else
			{
				if (!bIsDash)
				{
					FSMSet_Boss(EBossMovementStatus::EMS_MoveToTarget, MainPlayer);
				}
			}
		}
	}
}

void AEnemyBoss::BeginOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if(MainPlayer)
		{

			SetNowAttackDistance(EDistanceInfomation::EDI_Almost);
		}
	}
}

void AEnemyBoss::EndOnOverlapCombatSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{

			SetNowAttackDistance(EDistanceInfomation::EDI_Middle);
		}
	}
}

#pragma endregion 

#pragma region Pattern
void AEnemyBoss::SetAttackInfo()
{
	int RandomNum = 0;
	switch (NowAttackDistance)
	{
	case EDistanceInfomation::EDI_Almost:
		RandomNum = FMath::RandRange(0, 2);
		switch (RandomNum)
		{
		case 0:
			NowAttack = &Attack_OneHitCircle;
			break;
		case 1:
			NowAttack = &Attack_OneHitUltimate;
			break;
		case 2:
			NowAttack = &Attack_ThreeAttack;
			break;
		}
		break;

	case EDistanceInfomation::EDI_Middle:
		RandomNum = FMath::RandRange(0, 1);
		switch (RandomNum)
		{
		case 0:
			NowAttack = &Attack_EarthQuake;
			break;
		case 1:
			NowAttack = &Attack_SummonBomb;
			break;
		}
		break;

	case EDistanceInfomation::EDI_Range:
		RandomNum = FMath::RandRange(0, 1);
		switch (RandomNum)
		{
		case 0:
			NowAttack = &Attack_SummonBomb;
			break;
		case 1:
			NowAttack = &Attack_DeathBall;
			break;
		}
		break;
	default:
		break;
	}

	if (!CheckAttackCoolTime(*NowAttack))
	{
		NowAttack = &Attack_Empty;
	}
}

bool AEnemyBoss::CheckAttackCoolTime(FBossAttackInfo& AttackInfo)
{
	bool CanAttack = false;
	if (AttackInfo.NowAttackCoolTime >= AttackInfo.AttackCoolTime)
	{
		CanAttack = true;
	}
	return CanAttack;
}


void AEnemyBoss::SetBossAttackNowCoolTimes(float DeltaTime)
{
	Attack_OneHitCircle.NowAttackCoolTime += DeltaTime;
	Attack_OneHitUltimate.NowAttackCoolTime += DeltaTime;
	Attack_ThreeAttack.NowAttackCoolTime += DeltaTime;

	Attack_EarthQuake.NowAttackCoolTime += DeltaTime;
	Attack_SummonBomb.NowAttackCoolTime += DeltaTime;
	Attack_DeathBall.NowAttackCoolTime += DeltaTime;
}

void AEnemyBoss::PlayAttackMontage()
{
	if (NowAttack)
	{
		if (NowAttack->AttackMontage)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (AnimInstance)
			{
				AnimInstance->Montage_Play(NowAttack->AttackMontage);
			}
		}

		NowAttack->NowAttackCoolTime = 0;
	}
}

#pragma endregion

#pragma region Attack
void AEnemyBoss::StartAttack()
{

}

void AEnemyBoss::EndAttack()
{
	bIsAttack = false;
	FSMSet_Boss(AfterAttackChangeStatus, NowFSMTarget);
}

void AEnemyBoss::TrunWeaponCollider(bool IsOn)
{
	if (IsOn)
	{
		if (SwingSound)
		{
			UGameplayStatics::PlaySound2D(this, SwingSound);
		}
		WeaponCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	else
	{
		WeaponCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

#pragma region EarthQuake

void AEnemyBoss::EarthQuakeExplosion()
{
	FVector TipLocation = GetMesh()->GetSocketLocation("HammerTip");

	TArray<AActor*> OutActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); 
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Init(this, 1);

	UClass* SeekClass = AMainPlayer::StaticClass();

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), TipLocation, EarthQukeExplosionRadius, TraceObjectTypes, SeekClass, IgnoreActors, OutActors);

	for (int32 i = 0; i < OutActors.Num(); i++)
	{
		if (OutActors[i])
		{
			AMainPlayer* MainPlayer = Cast<AMainPlayer>(OutActors[i]);

			if (MainPlayer)
			{
				UE_LOG(LogTemp, Log, TEXT("MainPlayer"));
				UGameplayStatics::ApplyDamage(MainPlayer, NowAttack->AttackDamage, NULL, this, DamageTypeClass);

				USkeletalMeshComponent* HitCharacterMesh = MainPlayer->GetMesh();
				if (HitCharacterMesh)
				{
					FVector HitLocation = HitCharacterMesh->GetClosestCollidingRigidBodyLocation(GetActorLocation());
					MainPlayer->HitEvent(HitLocation);
				}
			}
		}
	}

	if (EarthQuakeParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EarthQuakeParticle, TipLocation, FRotator(0.0f), false);
	}

	if (CameraShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(CameraShake, 1.2f);
	}

	if (EarthQuakeSound)
	{
		UGameplayStatics::PlaySound2D(this, EarthQuakeSound);
	}

}

#pragma endregion 

#pragma region SummonBomb

void AEnemyBoss::SummonBomb()
{
	if(Bomb)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParameter;
		FVector CastLocation = GetMesh()->GetSocketLocation(FName("CastTip"));
		if (World)
		{
			AActor* BombActor = World->SpawnActor<AActor>(Bomb, NowFSMTarget->GetActorLocation(), GetActorRotation(), SpawnParameter);

			if (BombActor)
			{
				ABomb* CreateBomb = Cast<ABomb>(BombActor);
				
				if (CreateBomb)
				{
					NowBomb = CreateBomb;
				}
			}
		}

		if (BombSummonEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BombSummonEffect, CastLocation);
		}
	}
}

void AEnemyBoss::ExplosionBomb()
{
	if (NowBomb)
	{
		NowBomb->Explosion(NowAttack->AttackDamage);
		NowBomb = nullptr;
	}

	if (CameraShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(CameraShake, 1.2f);
	}
}

#pragma endregion

#pragma region DeathBall

void AEnemyBoss::ShootDeathBall()
{
	FVector CastLocation = GetMesh()->GetSocketLocation(FName("CastTip"));
	if (DeathBall)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParameter;

		if (World)
		{
			AActor* DeathBallActor = World->SpawnActor<AActor>(DeathBall, CastLocation, GetActorRotation(), SpawnParameter);

			if (DeathBallActor)
			{
				ADeathBall* DeathBallObject = Cast<ADeathBall>(DeathBallActor);
				
				if (DeathBallObject)
				{
					DeathBallObject->SetBallObject(NowFSMTarget, NowAttack->AttackDamage);
				}
			}
		}
	}

	if (DeathBallShootEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathBallShootEffect, CastLocation);
	}
}

#pragma endregion

#pragma endregion

float AEnemyBoss::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return DamageAmount;
	}
	CharacterStats.Health -= DamageAmount;
	if (CharacterStats.Health <= 0)
	{
		FSMSet_Boss(EBossMovementStatus::EMS_Dead);
		bIsDead = true;
		EndDeathEvent();
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySound2D(this, HitSound);
	}
	return DamageAmount;
}

void AEnemyBoss::HitEvent(FVector HitPose)
{
	if (HitMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		//AnimInstance->MontageBlenmd
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