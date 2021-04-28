#include "DeathBall.h"
#include "AIController.h"
#include "MainPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ADeathBall::ADeathBall()
{
	PrimaryActorTick.bCanEverTick = true;

	MoveSpeed = 100.f;	
	NowTime = 0.f;
	DestroyTime = 10.f;
	
	HitCollision = CreateDefaultSubobject<USphereComponent>(TEXT("HitCollision"));

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(HitCollision);
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.0f;
}

void ADeathBall::BeginPlay()
{
	Super::BeginPlay();


	HitCollision->OnComponentBeginOverlap.AddDynamic(this, &ADeathBall::BeginOnOverlapAttackSphere);
}

void ADeathBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveToTarget(DeltaTime);

	NowTime += DeltaTime;
	if (NowTime >= DestroyTime)
	{
		Destroy(this);
	}
}

void ADeathBall::MoveToTarget(float DeltaTime)
{
	if (TargetActor)
	{
		//FVector WantedDir = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();

		ProjectileMovementComponent->HomingTargetComponent = TargetActor->GetRootComponent();

		//WantedDir += TargetActor->GetVelocity() * WantedDir.Size() / ProjectileMovementComponent->MaxSpeed;

		FVector Velocity = ProjectileMovementComponent->Velocity;
		ProjectileMovementComponent->Velocity = FVector(Velocity.X, Velocity.Y, UpZPose);
		ProjectileMovementComponent->HomingTargetComponent = TargetActor->GetRootComponent();
		//ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity.GetSafeNormal() * ProjectileMovementComponent->MaxSpeed;
	}
	if (ProjectileMovementComponent->HomingTargetComponent == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Non HomingTarget"));
	}

	else
	{
		UE_LOG(LogTemp, Log, TEXT("HaveHomingTarget"));
	}
}

void ADeathBall::BeginOnOverlapAttackSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			UGameplayStatics::ApplyDamage(MainPlayer, AttackDamage, NULL, this, DamageTypeClass);

			USkeletalMeshComponent* HitCharacterMesh = MainPlayer->GetMesh();
			if (HitCharacterMesh)
			{
				FVector HitLocation = HitCharacterMesh->GetClosestCollidingRigidBodyLocation(GetActorLocation());
				MainPlayer->HitEvent(HitLocation);
			}
			UE_LOG(LogTemp, Log, TEXT("%s"), *MainPlayer->GetName());
			Destroy(this);
		}
	}
}

void ADeathBall::SetBallObject(AActor* Target, float Damage)
{
	if (Target)
	{
		ProjectileMovementComponent->HomingTargetComponent = Target->GetRootComponent();
	}
	TargetActor = Target;
	AttackDamage = Damage;
}