// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"
#include "MainPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ExplosiveRadius = 100.f;
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABomb::Explosion(float Damage)
{
	FVector TipLocation = GetActorLocation();

	TArray<AActor*> OutActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Init(this, 1);

	UClass* SeekClass = AMainPlayer::StaticClass();

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), TipLocation, ExplosiveRadius, TraceObjectTypes, SeekClass, IgnoreActors, OutActors);

	for (int32 i = 0; i < OutActors.Num(); i++)
	{
		if (OutActors[i])
		{
			AMainPlayer* MainPlayer = Cast<AMainPlayer>(OutActors[i]);

			if (MainPlayer)
			{
				// 여기 피해 주기
				UGameplayStatics::ApplyDamage(MainPlayer, Damage, NULL , this, DamageTypeClass);
				UE_LOG(LogTemp, Log, TEXT("메인 맞음!!"));
			}
		}
	}

	if (ExplosiveParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosiveParticle, TipLocation, FRotator(0.0f), false);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySound2D(this, ExplosionSound);
	}


	Destroy(this);
}