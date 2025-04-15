// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Interface/SaveActorInterface.h"
#include "SurvivalSaveGame.generated.h"

UCLASS()
class SURVIVAL_API USurvivalSaveGame : public USaveGame
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<FGuid, FSaveActorData> SaveableActorData;
	UPROPERTY()
	FName CurrentlyLoadedLevel = "NONE";

public:
	void SetSaveActorData(TMap<FGuid, FSaveActorData> Data);
	TMap<FGuid, FSaveActorData> GetSaveActorData();
	void SetCurrentLevel(const FName Level);
	FName GetCurrentLevel();
	
};
