// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "SoulLikeCharacterBase.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	HitCollision = CreateDefaultSubobject<USphereComponent>(TEXT("HitCollision"));
	RootComponent = HitCollision;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(HitCollision);
	ProjectileMovementComponent->InitialSpeed = 3000.f;
	ProjectileMovementComponent->MaxSpeed = 3000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.0f;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	HitCollision->OnComponentBeginOverlap.AddDynamic(this, &ABullet::BeginOnOverlapBulletSphere);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::SetDamage(float Damage)
{
	BulletDamage = Damage;
}

void ABullet::BeginOnOverlapBulletSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (!ShootActor)
		{
			return;
		}
		ASoulLikeCharacterBase* HitCharacter = Cast<ASoulLikeCharacterBase>(OtherActor);
		if (OtherActor != ShootActor)
		{
			if (HitCharacter)
			{
				UGameplayStatics::ApplyDamage(HitCharacter, BulletDamage, NULL, this, DamageTypeClass);

				USkeletalMeshComponent* HitCharacterMesh = HitCharacter->GetMesh();
				if (HitCharacterMesh)
				{
					FVector HitLocation = HitCharacterMesh->GetClosestCollidingRigidBodyLocation(GetActorLocation());
					HitCharacter->HitEvent(HitLocation);
				}
				UE_LOG(LogTemp, Log, TEXT("%s"), *HitCharacter->GetName());
				Destroy(this);
			}
		}
	}
}

void ABullet::FireInDirection(const FVector FireDirection)
{
	//ProjectileMovementComponent->Velocity = FireDirection * ProjectileMovementComponent->InitialSpeed;
}

void ABullet::SetShootActor(AActor* Actor)
{
	ShootActor = Actor;
}