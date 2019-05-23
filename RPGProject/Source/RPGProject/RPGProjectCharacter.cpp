// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "RPGProjectCharacter.h"
#include "Engine/Engine.h"
#include "Engine/Public/TimerManager.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ARPGProjectCharacter

ARPGProjectCharacter::ARPGProjectCharacter()
{
	
	// Enable Tick Event.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);	

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)	
}


//////////////////////////////////////////////////////////////////////////
// Input

void ARPGProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{ 	
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARPGProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARPGProjectCharacter::MoveRight);

	// Set up Casting key binding
	PlayerInputComponent->BindAction("Cast1H", IE_Pressed, this, &ARPGProjectCharacter::CastSpell1H);

	/** Commented out becaue we want to wait for the end of animation of CastSpell1H. (We dont want to change Casting1H midway.)*/
	//PlayerInputComponent->BindAction("Cast1H", IE_Released, this, &ARPGProjectCharacter::StopCastingSpell1H);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARPGProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARPGProjectCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ARPGProjectCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ARPGProjectCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ARPGProjectCharacter::OnResetVR);
}

void ARPGProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	PassiveRegeneration(HealthTempTimeHandle, Health, HealthRegeneration, HealthRegenerationRate, DeltaSeconds);
	PassiveRegeneration(ManaTempTimeHandle, Mana, ManaRegeneration, ManaRegenerationRate, DeltaSeconds);
}

void ARPGProjectCharacter::PassiveRegeneration(float& TempTimeHandle, float& Atribute, float AtributeRegeneration, float AtributeRegenerationRate, float DeltaSeconds)
{		
	TempTimeHandle += DeltaSeconds;
	if (AtributeRegenerationRate <= TempTimeHandle)
	{
		TempTimeHandle = 0.f;
		if (Atribute < 1.f) // 1 is equal to 100%
		{			
			Atribute += AtributeRegeneration;			
			if (Atribute > 1.f)
			{
				Atribute = 1.f;
			}
		}
	}
}

// TODO Fix stoping in midair when casting spell (stop movement immediately();) 
void ARPGProjectCharacter::CastSpell1H()
{		// TODO Eliminate Magic number. Mana cost. 
	if (!Casting1H && Mana>=0.15f) 
	{		
		GetCharacterMovement()->Deactivate();
		GetCharacterMovement()->StopMovementImmediately();
		
		Casting1H = true;
		// SpawnEmitterAtLocation is in character Eve_BP (effect of healing spell, particle) 
		// to avoid direct content references in C++ 
		
		// Seting up a Timer. We are waiting for an end of animation to change Casting1H back to false.	
		GetWorld()->GetTimerManager().SetTimer(CastSpell1HTimerHandle, this, &ARPGProjectCharacter::StopCastingSpell1H, 2.2f, false);		
	}	
}

void ARPGProjectCharacter::StopCastingSpell1H()
{
	Casting1H = false;
	// TODO Eliminate Magic number. Spell effect.
	Mana -= 0.15f;
	Health += 0.15f;
	GetCharacterMovement()->Activate();
}

void ARPGProjectCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ARPGProjectCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ARPGProjectCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ARPGProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ARPGProjectCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
