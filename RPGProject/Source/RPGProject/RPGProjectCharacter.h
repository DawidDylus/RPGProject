// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGProjectCharacter.generated.h"

UCLASS(config=Game)
class ARPGProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

private:

	FTimerHandle TimerHandle;
	FTimerHandle HealthRateTimerHandle;
	FTimerHandle ManaRateTimerHandle;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
		float Health = 0.5f; // Health in procentage 1 equal to 100%

	UPROPERTY(VisibleAnywhere, Category = "Stats")
		float Mana = 0.75f; // Mana in procentage 1 equal to 100%

	UPROPERTY(VisibleAnywhere, Category = "Casting")
		bool Casting1H;	
	
	UPROPERTY(EditAnywhere, Category = "Stats")
		float HealthRegeneration = 0.05f; // Health regeneration in procentage

	UPROPERTY(EditAnywhere, Category = "Stats")
		float ManaRegeneration = 0.05f; // Mana regeneration in procentage

	UPROPERTY(EditAnywhere, Category = "Stats")
		float HealthRegenerationRate = 1.f; // Time in sec that PassiveHealthRegeneration function will take effect

	UPROPERTY(EditAnywhere, Category = "Stats")
		float ManaRegenerationRate = 1.f;	// Time in sec that PassiveManaRegeneration function will take effect	

public:
	ARPGProjectCharacter();	

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UFUNCTION(BlueprintPure, Category = "Stats")
		float GetHealth() { return Health; }

	UFUNCTION(BlueprintPure, Category = "Stats")
		float GetMana() { return Mana; }

	UFUNCTION(BlueprintPure, Category = "Casting")
		bool GetCasting1H() { return  Casting1H; }	

/*
	UFUNCTION(BlueprintPure, Category = "Stats")
		float GetHealthRegeneration() { return HealthRegeneration; }

	UFUNCTION(BlueprintPure, Category = "Stats")
		float GetManaRegeneration() { return ManaRegeneration; }

*/

protected:

	virtual void BeginPlay() override;

	/** Regenerate Health based on HealthRegeneration variable */
	void PassiveHealthRegeneration();

	/** Regenerate Mana based on ManaRegeneration variable */
	void PassiveManaRegeneration();

	/** Cast spell 1H changing bool Cast1H to true*/
	void CastSpell1H();
	void StopCastingSpell1H();

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

