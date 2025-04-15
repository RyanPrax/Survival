// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SurvivalGameInstance.h"
#include "Game/SurvivalSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

USurvivalGameInstance::USurvivalGameInstance()
{

}

void USurvivalGameInstance::CreateSaveSlot()
{
	SaveGameObject = Cast<USurvivalSaveGame>(UGameplayStatics::CreateSaveGameObject(USurvivalSaveGame::StaticClass()));
}

void USurvivalGameInstance::GatherActorData()
{
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		// if in garbage collection or our actor doesn't use our save interface
		if (!IsValid(Actor) || !Actor->Implements<USaveActorInterface>())
		{
			continue;
		}
		// Whatever class the actor is, we get whatever information we can from the interface
		ISaveActorInterface* Inter = Cast<ISaveActorInterface>(Actor);
		// Failsafe, should never happen
		if (Inter == nullptr)
		{
			continue;
		}
		//TODO Next Episode

	}
}

void USurvivalGameInstance::AddActorData(const FGuid& ActorID, FSaveActorData ActorData)
{
	SaveableActorData[ActorID] = ActorData;
}

FSaveActorData USurvivalGameInstance::GetActorData(const FGuid& ActorID)
{
	return SaveableActorData[ActorID];
}
