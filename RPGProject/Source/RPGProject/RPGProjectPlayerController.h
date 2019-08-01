// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGProjectPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API ARPGProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;	

public:
	class ARPGProjectCharacter* GetPossesedCharacter() const;
};
