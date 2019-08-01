// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InputComponent.h"
#include "RPGProject/RPGProjectCharacter.h"
#include "RPGProjectPlayerController.h"

void ARPGProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

ARPGProjectCharacter* ARPGProjectPlayerController::GetPossesedCharacter() const
{
	return Cast<ARPGProjectCharacter>(this->GetPawn());
}


