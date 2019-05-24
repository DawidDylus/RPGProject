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
	FTimerHandle CastSpell1HTimerHandle;	

	UPROPERTY(EditAnywhere, Category = "Stats")
		float Health = 0.5f; // Health in percentage 1 equal to 100%

	UPROPERTY(EditAnywhere, Category = "Stats")
		float Mana = 0.75f; // Mana in percentage 1 equal to 100%

	UPROPERTY(VisibleAnywhere, Category = "Casting")
		bool Casting1H;	
	
	UPROPERTY(EditAnywhere, Category = "Stats")
		float HealthRegeneration = 0.01f; // Health regeneration in procentage

	UPROPERTY(EditAnywhere, Category = "Stats")
		float ManaRegeneration = 0.01f; // Mana regeneration in procentage

	UPROPERTY(EditAnywhere, Category = "Stats")
		float HealthRegenerationRate = 1.f; // Time in sec that PassiveHealthRegeneration function will take effect

	UPROPERTY(EditAnywhere, Category = "Stats")
		float ManaRegenerationRate = 1.f;	// Time in sec that PassiveManaRegeneration function will take effect	

	UPROPERTY()
	float HealthTempTimeHandle = 0.f;

	UPROPERTY()
	float ManaTempTimeHandle = 0.f;

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

	UFUNCTION(BlueprintCallable, Category = "Stats")
		void SetHealth(float NewHealth) { Health = NewHealth; }

	UFUNCTION(BlueprintPure, Category = "Stats")
		float GetMana() { return Mana; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
		void SetMana(float NewMana) { Mana = NewMana; }

	UFUNCTION(BlueprintPure, Category = "Casting")
		bool GetCasting1H() { return  Casting1H; }	

	UFUNCTION(BlueprintPure, Category = "Stats")
		float GetHealthRegeneration() { return HealthRegeneration; }

	UFUNCTION(BlueprintPure, Category = "Stats")
		float GetManaRegeneration() { return ManaRegeneration; }	

public:

	virtual void Tick(float DeltaSeconds) override;

	/** Regenerate a variable based on this variable regeneration value
	*@param TempTimeHandle Unique handle for time. Similar to TimerHandle.
	*@param Atribute regenerate in percents (e.g. Health)
	*@param AtributeRegeneration Value at whitch Atribute will be regenerated in percents (e.g. HealthRegeneration)
	*@param AtributeRegenerationRate Time between each regeneration(e.g. HealthRegenerationRate)
	*@param DeltaSeconds passed from TickEvent, necessary to calculations (count seconds passed in game so that AtributeREgenerationRate can work properly) 	
	*/
	UFUNCTION()
	void PassiveRegeneration(float& TempTimeHandle, float& Atribute, float AtributeRegeneration, float AtributeRegenerationRate, float DetlaSeconds);

protected:
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


