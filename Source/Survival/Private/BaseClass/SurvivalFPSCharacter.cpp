// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseClass/SurvivalFPSCharacter.h"
#include "Components/StatlineComponent.h"

// Sets default values
ASurvivalFPSCharacter::ASurvivalFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Statline = CreateDefaultSubobject<UStatlineComponent>(TEXT("Statline"));
	Statline->SetMovementCompReference(GetCharacterMovement());
}


// Called when the game starts or when spawned
void ASurvivalFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ASurvivalFPSCharacter::CanJump() const
{
	return Statline->CanJump();
}


void ASurvivalFPSCharacter::HasJumped()
{
	Statline->HasJumped();
	ACharacter::Jump();
}

bool ASurvivalFPSCharacter::CanSprint() const
{
	return Statline->CanSprint();
}

void ASurvivalFPSCharacter::SetSprinting(const bool& IsSprinting)
{
	Statline->SetSprinting(IsSprinting);
}


void ASurvivalFPSCharacter::SetSneaking(const bool& IsSneaking)
{
	Statline->SetSneaking(IsSneaking);
}

// Called every frame
void ASurvivalFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASurvivalFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

