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
	FTimerHandle CastSpellAnimationTimerHandle;
	
	
	float HealthTimeHandle;	
	float ManaTimeHandle;

public:
	ARPGProjectCharacter();	

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseLookUpRate;

	// Health in percentage 1 equal to 100%	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Atributes")
		float HealthPercentage; 

	// Mana in percentage 1 equal to 100%	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Atributes")
		float ManaPercentage; 	

	// Value in percentage of which Health will be regenerated
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Atributes")
		float HealthRegeneration; 

	// Value in percentage of which Mana will be regenerated
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Atributes")
		float ManaRegeneration; 

	// Time in sec between each change of Health variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Atributes")
		float HealthRegenerationRate;

	// Time in sec between each change of Mana variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Atributes")
		float ManaRegenerationRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Casting")
		bool bPlaying1HCastingAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Casting")
		bool bCurrentlyCasting;

	UPROPERTY(EditAnywhere, Category = "Spells Visual Effects")
		class UParticleSystem* HealingParticleTemplate;

public:
	virtual void Tick(float DeltaSeconds) override;

	/** Regenerate a variable based on this variable regeneration value
	*@param OutTimePassedHandle - Unique handle for time. Similar to TimerHandle.
	*@param OutAtribute - Atribute to regenerate in percentage (e.g. Health)
	*@param ValueOfRegeneration - Value at whitch Atribute will be regenerated in percents (e.g. HealthRegeneration)
	*@param AtributeRegenerationRate - Time between each regeneration(e.g. HealthRegenerationRate)	
	*@warning - Function should be called only in tick function
	*/
	UFUNCTION()
	void PassiveRegeneration(float& OutTimePassedHandle, float& OutAtribute, const float& ValueOfRegeneration, const float& AtributeRegenerationRate);

	/** Adds value to selected variable;
	*@param OutAttribute - Variable to change
	*@param ValueToAdd - Value by whitch variable will be changed
	*/
	UFUNCTION()
	void ChangeAttribute(float& OutAttribute, const float& ValueToAdd);

private:
	/** Returns true when certain time passed 
	*@param OutTimePassed - variable that will store time passed in game calculated from delta seconds.
	*@param TimeToTrigger - time in seconds to return true and zeroed OutTimePassed value
	*@warning - Function should be called only in tick function
	*/
	bool ShouldTriggerFunction(float& OutTimePassed, const float& TimeToTrigger);

	void Play1HCastingAnimation();
	void OnTimerEnds();

protected:

	void CastHeal();

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
