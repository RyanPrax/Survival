// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SurvivalSaveGame.h"
#include "Interface/SaveActorInterface.h"

void USurvivalSaveGame::SetSaveActorData(TMap<FGuid, FSaveActorData> Data)
{
	SaveableActorData = Data;
}

TMap<FGuid, FSaveActorData> USurvivalSaveGame::GetSaveActorData()
{
	return SaveableActorData;
}

void USurvivalSaveGame::SetCurrentLevel(const FName Level)
{
	CurrentlyLoadedLevel = Level;
}

FName USurvivalSaveGame::GetCurrentLevel()
{
	return CurrentlyLoadedLevel;
}
