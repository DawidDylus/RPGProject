// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "RPGProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Public/TimerManager.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"

//////////////////////////////////////////////////////////////////////////
// ARPGProjectCharacter

ARPGProjectCharacter::ARPGProjectCharacter()
{	
	// Enable Tick Event.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);	

	// Set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Set basic Attributes for character
	HealthPercentage = 0.5f;
	HealthRegenerationRate = 1.f;
	ManaPercentage = 0.75f;
	ManaRegenerationRate = 1.f;
	HealthRegeneration = 0.01f;
	ManaRegeneration = 0.01f;
	
	// Casting
	bCurrentlyCasting = false;

	// Play animation if true
	bPlaying1HCastingAnimation = false;

	// Set particle templates for spells (effects) - nullptr for safety, change in child BP class.
	HealingParticleTemplate = nullptr;	

	// Dependency for calculating PassedTime variable in PassiveRegeneration()
	HealthTimeHandle = 0.f;
	ManaTimeHandle = 0.f;

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
	PlayerInputComponent->BindAction("Cast1H", IE_Pressed, this, &ARPGProjectCharacter::CastHeal);

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
	
	PassiveRegeneration(HealthTimeHandle, HealthPercentage, HealthRegeneration, HealthRegenerationRate);
	PassiveRegeneration(ManaTimeHandle, ManaPercentage, ManaRegeneration, ManaRegenerationRate);
}


void ARPGProjectCharacter::PassiveRegeneration(float& OutTimePassedHandle, float& OutAtribute, const float& ValueOfRegeneration, const float& AtributeRegenerationRate)
{		
	if (ShouldTriggerFunction(OutTimePassedHandle, AtributeRegenerationRate))
	{
		ChangeAttribute(OutAtribute, ValueOfRegeneration);
	}
}

void ARPGProjectCharacter::ChangeAttribute(float& OutAttribute, const float& ValueToAdd)
{
	static const float HundredPercent = 1.f;
	if (OutAttribute < HundredPercent) 
	{
		OutAttribute += ValueToAdd;
		if (OutAttribute > HundredPercent)
		{
			OutAttribute = HundredPercent;
		}
	}
}

bool ARPGProjectCharacter::ShouldTriggerFunction(float& OutTimePassed, const float& TimeToTrigger)
{
	OutTimePassed += GetWorld()->GetDeltaSeconds();
	if (OutTimePassed >= TimeToTrigger) 
	{
		OutTimePassed = 0.f;
		return true;
	}
	else
	{
		return false;
	}		
}

void ARPGProjectCharacter::Play1HCastingAnimation()
{	
	const float AnimationDuration = 2.2f;
	bPlaying1HCastingAnimation = true;
	GetWorldTimerManager().SetTimer(CastSpellAnimationTimerHandle, this, &ARPGProjectCharacter::OnTimerEnds, AnimationDuration, false);	
}

void ARPGProjectCharacter::OnTimerEnds()
{
	bPlaying1HCastingAnimation = false;
}

void ARPGProjectCharacter::CastHeal()
{	
	if (!GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying() && !GetCharacterMovement()->IsSwimming())
	{
		// deactivate character movement  input	 if active movement only turn it off for a sec
		// GetCharacterMovement()->Deactivate();
		GetCharacterMovement()->StopActiveMovement();
		GetCharacterMovement()->StopMovementImmediately();

		// TODO declare those variables in .h so that they can be modified in ue4
		const float ManaCost = 0.15f;
		const float HealingValue = 0.15f;
		if (!bCurrentlyCasting && ManaPercentage >= ManaCost)
		{			
			ManaPercentage -= ManaCost;

			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				HealingParticleTemplate,
				GetMesh()->GetSocketLocation("RightFoot"),
				GetActorRotation(),
				true
			);			
			Play1HCastingAnimation();

			// TODO Check if health is changed after animation or before
				// use while to wait for variable to change (to false)
				// or reserch more about async #include "Async/Future.h"
			HealthPercentage += HealingValue;

			//GetCharacterMovement()->Activate();
		}

	}
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
