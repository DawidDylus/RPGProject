// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "Components/CapsuleComponent.h"
#include "RPGProjectCharacter.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AHealthPickup::AHealthPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(44.f, 33.f);
	this->SetRootComponent(CapsuleComponent);	

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(CapsuleComponent);

	// ParticleSystemComponent references are set in the derived blueprint asset named HealthPickup_BP
	// (to avoid direct content references in C++)	
}

// Called when the game starts or when spawned
void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
	// Call Function PickupEffect when actors will overlap
	this->OnActorBeginOverlap.AddDynamic(this, &AHealthPickup::PickupEffect);	
}

// Called every frame
void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealthPickup::PickupEffect(AActor* MyOverlappedActor, AActor* OtherActor)
{	
	ARPGProjectCharacter* OtherOverlappedActor = Cast<ARPGProjectCharacter>(OtherActor);
	if (OtherOverlappedActor)
	{		
		float Health = OtherOverlappedActor->GetHealth();
		if (Health < 1.f)
		{
			Health += HealValue;
			if (Health > 1.f)
			{
				Health = 1.f;
			}
			OtherOverlappedActor->SetHealth(Health);

			if (!ParticleSpawnAtTarget)
			{
				UE_LOG(LogTemp, Warning, TEXT("Template for ParticleSpawnAtTarget is not found!"));
				Destroy();
				GEngine->ForceGarbageCollection();
			}
			else
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ParticleSpawnAtTarget,
					OtherOverlappedActor->GetActorLocation(),
					OtherOverlappedActor->GetActorRotation(),
					true
				);
				Destroy();
				GEngine->ForceGarbageCollection();
			}
		}
	}	
}

