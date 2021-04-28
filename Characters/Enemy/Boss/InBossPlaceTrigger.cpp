// Fill out your copyright notice in the Description page of Project Settings.


#include "InBossPlaceTrigger.h"
#include "Components/BoxComponent.h"
#include "MainPlayer.h"
#include "EnemyBoss.h"

// Sets default values
AInBossPlaceTrigger::AInBossPlaceTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TransitionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TransitionVolume->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AInBossPlaceTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	TransitionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInBossPlaceTrigger::OnOverlapBegin);
}

void AInBossPlaceTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainPlayer* Main = Cast<AMainPlayer>(OtherActor);
		if (Main)
		{
			// UI 업데이트
			if (BossObject)
			{
				BossObject->FSMSet_Boss(EBossMovementStatus::EMS_MoveToTarget, Main);
			}

			Main->SetBoss(BossObject);
		}
	}
}
