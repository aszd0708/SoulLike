// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"
#include "EnemyBase.h"
#include "EnemyBoss.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"
#include "Engine/SkeletalMeshSocket.h"

#include "TargetSystemComponent.h"


AMainPlayer::AMainPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	CharacterStats.Health = 200;
	CharacterStats.MaxHealth = 200;
	CharacterStats.Stamina = 100;
	CharacterStats.MaxStamina = 100;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f);

	// 카메라붐 생성(충돌발생시 카메라를 플레이어로 땡김)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// 캡슐 충돌크기 설정
	GetCapsuleComponent()->SetCapsuleSize(48.0f, 105.0f);

	// 따라오는 카메라 생성 
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	BaseTurnRate = 65.0f;
	BaseLookUpRate = 65.0f;

	// 캐릭터를 입력 방향으로 회전
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;
	SprintingStamina = 10.f;

	NowRollingButtonPushTime = 0;
	RollingButtonPushTime = 0.25f;
	RollingDistance = 50.f;
	RollingHeight = 0.5f;
	RollingStamina = 30.f;
	bIsLanding = false;

	NowAttackIndex = 0;

	PlayerMovementStatus = EMovementStatus::EMS_Normal;
	PlayerBehaviorStatus = EBehaviorStatus::EBS_Normal;

	bIsAttacking = false;

	AttackSpeed = 1.2f;
	AttackDamage = 50.f;
	AttackStamina = 30.f;

	bIsHit = false;
	HitRecoverTime = 0.3f;
	NowHitRecoverTime = 0.0f;

	bIsLockOn = false;

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponCollision"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftCollisionSocket"));
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollision"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightCollisionSocket"));
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EnemyTargetCollision = CreateDefaultSubobject<USphereComponent>("EnemyTargetCollision");
	EnemyTargetCollision->SetupAttachment(GetRootComponent());
	EnemyTargetCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	EnemyTargetCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	EnemyTargetCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	TargetSystem = CreateDefaultSubobject<UTargetSystemComponent>("TargetSystem");
}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();

	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AMainPlayer::BeginOnOverlapWeapon);
	LeftWeaponCollision->OnComponentEndOverlap.AddDynamic(this, &AMainPlayer::EndOnOverlapWeapon);

	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AMainPlayer::BeginOnOverlapWeapon);
	RightWeaponCollision->OnComponentEndOverlap.AddDynamic(this, &AMainPlayer::EndOnOverlapWeapon);

	EnemyTargetCollision->OnComponentBeginOverlap.AddDynamic(this, &AMainPlayer::BeginOnOverlapEnemyTarget);
	EnemyTargetCollision->OnComponentEndOverlap.AddDynamic(this, &AMainPlayer::EndOnOverlapEnemyTarget);
}

#pragma region KeyBinding
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("RollingAndSprint", IE_Pressed, this, &AMainPlayer::PushShift);
	PlayerInputComponent->BindAction("RollingAndSprint", IE_Released, this, &AMainPlayer::PullShift);

	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &AMainPlayer::LockOnEnemy);
	PlayerInputComponent->BindAction("ChangeLockOnTargetLeft", IE_Pressed, this, &AMainPlayer::ChangeTargetLeft);
	PlayerInputComponent->BindAction("ChangeLockOnTargetRifht", IE_Pressed, this, &AMainPlayer::ChangeTargetRight);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainPlayer::Attack);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainPlayer::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMainPlayer::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainPlayer::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainPlayer::LookUpAtRate);
}

void AMainPlayer::CheckMovementStatus()
{
	switch (PlayerMovementStatus)
	{
	case EMovementStatus::EMS_Normal:
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		break;

	case EMovementStatus::EMS_Sprinting:
		if (!MinusStamina(SprintingStamina))
		{
			SetPlayerMovementStatus(EMovementStatus::EMS_Normal);
			GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
		}
		break;

	case EMovementStatus::EMS_Dead:
		break;

	case EMovementStatus::EMS_Hit:
		bIsHit = true;
		break;

	default:
		break;
	}
}

void AMainPlayer::CheckBehaviorStatus()
{
	switch (PlayerBehaviorStatus)
	{
	case EBehaviorStatus::EBS_Normal:
		break;

	case EBehaviorStatus::EBS_Attacking:
		break;

	case EBehaviorStatus::EBS_Avoiding:
		break;

	case EBehaviorStatus::EBS_Dead:
		break;

	default:
		break;
	}
}

#pragma region Movements
void AMainPlayer::Turn(float Value)
{
	if (bIsLockOn)
	{
		return;
	}
	AddControllerYawInput(Value);
}

void AMainPlayer::LookUp(float Value)
{
	if (bIsLockOn)
	{
		return;
	}
	AddControllerPitchInput(Value);
	//FollowCamera->AddLocalRotation
}

void AMainPlayer::MoveForward(float Value)
{
	//if (bIsRolling || bIsAttacking)
	//{
	//	return;
	//}

	if (GetPlayerBehaviorStatus() == EBehaviorStatus::EBS_Normal)
	{
		// 앞이 어디인지 찾음
		const FRotator Rotation = Controller->GetControlRotation();
		//const FRotator YawRoation(0.0f, 0.0f, 0.0f);
		const FRotator YawRoation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRoation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
	if (bIsLockOn)
	{
	}
	TargetingForwardValue = Value;
}

void AMainPlayer::MoveRight(float Value)
{
	//if (bIsRolling || bIsAttacking)
	//{
	//	return;
	//}

	if (GetPlayerBehaviorStatus() == EBehaviorStatus::EBS_Normal)
	{
		// 앞이 어디인지 찾음
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRoation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRoation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}

	if (bIsLockOn)
	{
	}
	TargetingRightValue = Value;
}

void AMainPlayer::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate *GetWorld()->GetDeltaSeconds());
}

void AMainPlayer::LookUpAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseLookUpRate *GetWorld()->GetDeltaSeconds());
}
#pragma endregion

void AMainPlayer::PushShift()
{
	if (bIsRolling || bIsAttacking)
	{
		return;
	}
	bShiftKeyDown = true;
}

void AMainPlayer::PullShift()
{
	if (GetPlayerMovementStatus() != EMovementStatus::EMS_Dead)
	{
		bShiftKeyDown = false;
		NowRollingButtonPushTime = 0;
		SetPlayerMovementStatus(EMovementStatus::EMS_Normal);
	}
}

void AMainPlayer::CheckRollingButtonTime(float DeltaTime)
{
	if (bShiftKeyDown)
	{
		NowRollingButtonPushTime += DeltaTime;
		if (NowRollingButtonPushTime >= RollingButtonPushTime)
		{
			// 달리기로 변경
			SetPlayerMovementStatus(EMovementStatus::EMS_Sprinting);
			bIsRolling = false;
			checkRolling = false;
		}
		else
		{
			checkRolling = true;
		}
	}
	else
	{
		if (NowRollingButtonPushTime < RollingButtonPushTime && checkRolling && !GetCharacterMovement()->IsFalling())
		{
			Rolling(DeltaTime);
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("X : %f"), GetActorForwardVector().X);
	//UE_LOG(LogTemp, Log, TEXT("Y : %f"), GetActorForwardVector().Y);
}

void AMainPlayer::Rolling(float DeltaTime)
{


	if (MinusStamina(RollingStamina))
	{
		bIsRolling = true;
		SetPlayerBehaviorStatus(EBehaviorStatus::EBS_Avoiding);
		checkRolling = false;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && RollingMontage)
		{
			AnimInstance->Montage_Play(RollingMontage, 1.5f);
		}

		GetCharacterMovement()->BrakingFrictionFactor = 0.f;
		FVector Location = GetActorForwardVector();
		if (!bIsLockOn)
		{
			LaunchCharacter(FVector(Location.X, Location.Y, RollingHeight).GetSafeNormal() * RollingDistance, true, true);
		}

		else
		{
			FVector InputDirection = FVector(TargetingRightValue, -TargetingForwardValue, 0);
			Location = GetActorForwardVector() * TargetingForwardValue;
			FVector Calculate = FVector(Location.X, Location.Y, RollingHeight).GetSafeNormal();

			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRoation(0.0f, Rotation.Yaw, 0.0f);
			const FRotator PitchRoation(Rotation.Pitch, 0.0f, Rotation.Roll);

			const FVector DirectionRight = (FRotationMatrix(YawRoation) * TargetingRightValue).GetUnitAxis(EAxis::Y);

			Calculate = (FVector(Location.X, Location.Y, RollingHeight) + DirectionRight);
			LaunchCharacter(Calculate.GetSafeNormal() * RollingDistance, true, true);
		}
	}
}

void AMainPlayer::EndRolling()
{
	SetIsRolling(false);
	SetPlayerBehaviorStatus(EBehaviorStatus::EBS_Normal);
	checkRolling = false;
}

void AMainPlayer::CheckLanding()
{
	if (checkRolling)
	{
		if (GetCharacterMovement()->IsFalling())
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(RollingMontage, 1.5f);
			}
		}
	}
}

#pragma endregion

#pragma region Targeting

void AMainPlayer::SetTarget()
{

}

#pragma endregion

// Called every frame
void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//bIsLockOn = TargetSystem->bEnableStickyTarget;
	PlayerDeltaTime = DeltaTime;
	CheckRollingButtonTime(DeltaTime);
	CheckComboAttack();
	CheckMovementStatus();
}

void AMainPlayer::GetDamage(float Damage)
{
	if (GetIsRolling())
	{
		return;
	}
}

void AMainPlayer::Attack()
{
	if (bIsRolling)
	{
		return;
	}
	if (GetPlayerMovementStatus() == EMovementStatus::EMS_Normal || GetPlayerMovementStatus() == EMovementStatus::EMS_Sprinting)
	{
		if (!bIsAttacking || bCheckComboAttack)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(AttackMontage, AttackSpeed);
				switch (NowAttackIndex)
				{
				case 0:
					//UE_LOG(LogTemp, Log, TEXT("Attack_0"));
					AnimInstance->Montage_JumpToSection("Attack_0");
					break;
				case 1:
					//UE_LOG(LogTemp, Log, TEXT("Attack_1"));
					AnimInstance->Montage_JumpToSection("Attack_1");
					break;
				}

				if (AttackSound)
				{
					UGameplayStatics::PlaySound2D(this, AttackSound);
				}
				SetWeaponCollider();

				NowAttackIndex++;
				NowAttackIndex %= 2;
				bIsAttacking = true;
				SetPlayerBehaviorStatus(EBehaviorStatus::EBS_Attacking);
				bCheckComboAttack = false;
				bDoComboAttack = false;
				AttackObjects.Empty();
			}
		}
	}

	bPressedLMB = true;
}

void AMainPlayer::EndAttack()
{
	bIsAttacking = false;
	SetPlayerBehaviorStatus(EBehaviorStatus::EBS_Normal);
	bDoComboAttack = false;
	NowAttackIndex = 0;
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMainPlayer::DoNextAttack()
{
	bDoComboAttack = true;
}

void AMainPlayer::CheckComboAttack()
{
	if (bDoComboAttack)
	{
		if (bPressedLMB)
		{
			bCheckComboAttack = true;
		}
	}
	else
	{
		bPressedLMB = false;
	}
}

void AMainPlayer::SetWeaponCollider()
{

	if (MinusStamina(AttackStamina))
	{
		UE_LOG(LogTemp, Log, TEXT("attacj : %d"), NowAttackIndex);
		switch (NowAttackIndex)
		{
		case 0:
			LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		case 1:
			LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			break;
		}
	}
}

void AMainPlayer::BeginOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOnOverlapWeapon(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);

		if (Enemy)
		{
			for (int i = 0; i < AttackObjects.Num(); i++)
			{
				if (AttackObjects[i] == Enemy)
				{
					return;
				}
			}
			AttackObjects.Add(Enemy);
			UE_LOG(LogTemp, Log, TEXT("EnemyHit!!"));
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, AttackDamage, GetController(), this, DamageTypeClass);
			}

			const USkeletalMeshSocket* TipSocket = nullptr;

			if (NowAttackIndex == 0)
			{
				TipSocket = GetMesh()->GetSocketByName("RightTipSocket");
			}
			else if (NowAttackIndex == 1)
			{
				TipSocket = GetMesh()->GetSocketByName("LeftTipSocket");
			}

			if (TipSocket)
			{
				FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
				FVector Location = Enemy->GetMesh()->GetClosestCollidingRigidBodyLocation(SocketLocation);
				Enemy->HitEvent(Location);
			}
		}
	}
}

void AMainPlayer::EndOnOverlapWeapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOnOverlapWeapon(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}

float AMainPlayer::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsRolling)
	{
		return DamageAmount;
	}
	if (bIsHit)
	{
		return DamageAmount;
	}
	CharacterStats.Health -= DamageAmount;

	if (CharacterStats.Health <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Die XD"));
	}
	else
	{
		SetPlayerMovementStatus(EMovementStatus::EMS_Hit);
	}
	return DamageAmount;
}

void AMainPlayer::HitEvent(FVector HitPose)
{
	if ((PlayerBehaviorStatus == EBehaviorStatus::EBS_Normal || PlayerBehaviorStatus == EBehaviorStatus::EBS_Attacking) && !bIsHit)
	{
		UE_LOG(LogTemp, Log, TEXT("X : %f Y : %f"), HitPose.X, HitPose.Y);
		if (HitParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitPose, FRotator(0.f), false);
		}

		if (HitSound)
		{
			UGameplayStatics::PlaySound2D(this, HitSound);
		}

		HitDirection = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), HitPose) * 180;

		if (HitMontage)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (HitDirection.X > 0)
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
}

void AMainPlayer::OffAttackCollision()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackObjects.Empty();
}

void AMainPlayer::StartHitEvent()
{
	SetPlayerMovementStatus(EMovementStatus::EMS_Hit);
	bIsHit = true;
}

void AMainPlayer::EndHitEvent()
{
	SetPlayerMovementStatus(EMovementStatus::EMS_Normal);

	if (PlayerBehaviorStatus == EBehaviorStatus::EBS_Attacking)
	{
		PlayerBehaviorStatus = EBehaviorStatus::EBS_Normal;
		bIsAttacking = false;
	}
	bIsHit = false;
}

void AMainPlayer::BeginOnOverlapEnemyTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
		if (Enemy)
		{
			UE_LOG(LogTemp, Log, TEXT("InEnemy"));
			//AEnemyBoss* Boss = Cast<AEnemyBoss>(Enemy);
			//if (!Boss)
			//{
				Enemise.Add(Enemy);
			//}
		}
	}
}

void AMainPlayer::EndOnOverlapEnemyTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (OtherActor)
		{
			AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
			if (Enemy)
			{
				UE_LOG(LogTemp, Log, TEXT("OutEnemy"));
				//AEnemyBoss* Boss = Cast<AEnemyBoss>(Enemy);
				//if (!Boss)
				//{
					Enemise.Remove(Enemy);
				//}
			}
		}
	}
}

void AMainPlayer::LockOnEnemy()
{
	if (bIsLockOn)
	{
		bIsLockOn = false;
		TargetSystem->TargetLockOff();
	}
	else
	{
		if (Enemise.Num() > 0)
		{
			bIsLockOn = true;
			//AActor* TargetActor = Cast<AActor>(Enemise[GetTargetIndex()]);
			//
			//if(TargetActor)
			//{
			//	TargetSystem->SetTargetActor(TargetActor);
			//}

			TargetSystem->TargetActor();

		}
		else
		{
			//UE_LOG(LogTemp, Log, TEXT("X : %f"), GetController()->GetControlRotation().Roll);
			//UE_LOG(LogTemp, Log, TEXT("Y : %f"), GetController()->GetControlRotation().Pitch);
			//UE_LOG(LogTemp, Log, TEXT("Z : %f"), GetController()->GetControlRotation().Yaw);

			bIsLockOn = false;
			TargetSystem->TargetLockOff();
		}
	}
}

void AMainPlayer::LockOffEnemy()
{
	bIsLockOn = false;
	TargetSystem->TargetLockOff();
}

int AMainPlayer::GetTargetIndex()
{
	if (TargetIndex >= Enemise.Num())
	{
		TargetIndex = 0;
	}

	return TargetIndex;
}

void AMainPlayer::ChangeTargetLeft()
{
	if (bIsLockOn) 
	{		
		TargetSystem->TargetActorWithAxisInput(1.0f);
	}
}

void AMainPlayer::ChangeTargetRight()
{
	if (bIsLockOn)
	{
		TargetSystem->TargetActorWithAxisInput(-1.0f);
	}
}

void AMainPlayer::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMainPlayer::SetBoss(AEnemyBoss* _Boss)
{
	Boss = _Boss;
}