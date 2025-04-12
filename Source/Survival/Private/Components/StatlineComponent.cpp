// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StatlineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UStatlineComponent::TickStats(const float DeltaTime)
{
	TickStamina(DeltaTime);
	TickHunger(DeltaTime);
	TickThirst(DeltaTime);
	// Don't regenerate health if hunger or thirst is <= 0.0
	// Otherwise they cancel out
	if (Thirst.GetCurrent() <= 0.0 || Hunger.GetCurrent() <= 0.0)
	{
		return;
	}
	Health.TickStat(DeltaTime);
}
void UStatlineComponent::TickStamina(const float DeltaTime)
{
	if (CurrentStaminaExhaustion > 0.0)
	{
		CurrentStaminaExhaustion -= DeltaTime;
		return;
	}

	if (bIsSprinting && IsValidSprinting()) 
	{
		Stamina.TickStat(0 - abs((DeltaTime * SprintCostMultiplier)));
		if (Stamina.GetCurrent() <= 0.0)
		{
			SetSprinting(false);
			CurrentStaminaExhaustion = SecondsForStaminaExhaustion;
		}
		return;
	}
	Stamina.TickStat(DeltaTime);
}

void UStatlineComponent::TickHunger(const float DeltaTime)
{
	if (Hunger.GetCurrent() <= 0.0)
	{
		Health.Adjust(0 - abs(StarvingHealthDamagePerSecond * DeltaTime));
		return;
	}

	Hunger.TickStat(DeltaTime);
}

void UStatlineComponent::TickThirst(const float DeltaTime)
{
	if (Thirst.GetCurrent() <= 0.0)
	{
		Health.Adjust(0 - abs(DehydrationHealthDamagePerSecond * DeltaTime));
		return;
	}

	Thirst.TickStat(DeltaTime);
}

bool UStatlineComponent::IsValidSprinting()
{
	return OwningCharacterMovementComp->Velocity.Length() > WalkSpeed && !OwningCharacterMovementComp->IsFalling();
}

// Sets default values for this component's properties
UStatlineComponent::UStatlineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStatlineComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningCharacterMovementComp->MaxWalkSpeed = WalkSpeed;
	
}


// Called every frame
void UStatlineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// Ensures decay does not happen when game is paused
	if (TickType != ELevelTick::LEVELTICK_PauseTick)
	{
		TickStats(DeltaTime);
	}
}

void UStatlineComponent::SetMovementCompReference(UCharacterMovementComponent* Comp)
{
	OwningCharacterMovementComp = Comp;
}

float UStatlineComponent::GetStatPercentile(const ECoreStat Stat) const
{
	switch (Stat)
	{
	case ECoreStat::CS_HEALTH:
		return Health.Percentile();
	case ECoreStat::CS_STAMINA:
		return Stamina.Percentile();
	case ECoreStat::CS_HUNGER:
		return Hunger.Percentile();
	case ECoreStat::CS_THIRST:
		return Thirst.Percentile();
	default:
		//TODO Log invalid stat
		break;
	}
	return -1;
}

bool UStatlineComponent::CanSprint() const
{
	return Stamina.GetCurrent() > 0.0;
}

void UStatlineComponent::SetSprinting(const bool IsSprinting)
{
	bIsSprinting = IsSprinting;
	if (bIsSneaking && !bIsSprinting)
	{
		return;
	}
	bIsSneaking = false;
	OwningCharacterMovementComp->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void UStatlineComponent::SetSneaking(const bool IsSneaking)
{
	bIsSneaking = IsSneaking;
	if (bIsSprinting && !bIsSneaking)
	{
		return;
	}
	bIsSprinting = false;
	OwningCharacterMovementComp->MaxWalkSpeed = bIsSneaking ? SneakSpeed : WalkSpeed;
}

bool UStatlineComponent::CanJump()
{
	return Stamina.GetCurrent() >= JumpCost && !OwningCharacterMovementComp->IsFalling();
}

void UStatlineComponent::HasJumped()
{
	Stamina.Adjust(0 - JumpCost);
}