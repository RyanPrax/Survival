// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StatlineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SurvivalUtils.h"

void UStatlineComponent::TickStats(const float& DeltaTime)
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
void UStatlineComponent::TickStamina(const float& DeltaTime)
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

void UStatlineComponent::TickHunger(const float& DeltaTime)
{
	if (Hunger.GetCurrent() <= 0.0)
	{
		Health.Adjust(0 - abs(StarvingHealthDamagePerSecond * DeltaTime));
		return;
	}

	Hunger.TickStat(DeltaTime);
}

void UStatlineComponent::TickThirst(const float& DeltaTime)
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

void UStatlineComponent::SetSprinting(const bool& IsSprinting)
{
	bIsSprinting = IsSprinting;
	if (bIsSneaking && !bIsSprinting)
	{
		return;
	}
	bIsSneaking = false;
	OwningCharacterMovementComp->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void UStatlineComponent::SetSneaking(const bool& IsSneaking)
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

FSaveComponentData UStatlineComponent::GetComponentSaveData_Implementation()
{
	FSaveComponentData Ret;
	Ret.ComponentClass = this->GetClass();
	Ret.RawData.Add(Health.GetSaveString()); //0
	Ret.RawData.Add(Stamina.GetSaveString()); //1
	Ret.RawData.Add(Hunger.GetSaveString()); //2
	Ret.RawData.Add(Thirst.GetSaveString()); //3
	// Any addition raw data adds here need to be included in the SetComponentSaveData_Implementation function

	return Ret;
}

void UStatlineComponent::SetComponentSaveData_Implementation(FSaveComponentData Data)
{
	TArray<FString> Parts;
	for (int i = 0; i < Data.RawData.Num(); i++)
	{
		Parts.Empty();
		Parts = ChopString(Data.RawData[i], '|');
		switch (i)
		{
		case 0:
			Health.UpdateFromSaveString(Parts);
			break;
		case 1:
			Stamina.UpdateFromSaveString(Parts);
			break;
		case 2:
			Hunger.UpdateFromSaveString(Parts);
			break;
		case 3:
			Thirst.UpdateFromSaveString(Parts);
			break;
		default:
			// Log error
			break;
		}
	}
}

