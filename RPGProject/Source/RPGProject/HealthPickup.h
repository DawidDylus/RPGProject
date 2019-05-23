// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickup.generated.h"

UCLASS()
class RPGPROJECT_API AHealthPickup : public AActor
{
	GENERATED_BODY()

	// ParticleSystem adds particles effect.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* ParticleSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CapsuleComponent, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComponent;

private: 
	UPROPERTY(EditAnywhere, Category = "Pickup Effects")
		float HealValue = 0.20f;

	UPROPERTY(EditAnywhere, Category = "Particle Template")
		class UParticleSystem* ParticleSpawnAtTarget = nullptr; // spawn when this Actor collide with ARPGProjectCharacter

public:	
	// Sets default values for this actor's properties
	AHealthPickup();

	UFUNCTION(BlueprintPure, Category = "Stats")
		float GetHealValue() { return HealValue; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void PickupEffect(AActor* MyOverlappedActor, AActor* OtherActor);

	/** Returns ParticleSystem subobject **/
	FORCEINLINE class UParticleSystemComponent* GetParticleSystem() const { return ParticleSystem; }

	/** Returns CapsuleComponent subobject **/
	FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }

};
