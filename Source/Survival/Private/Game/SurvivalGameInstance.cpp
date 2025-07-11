// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SurvivalGameInstance.h"
#include "Game/SurvivalSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>

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
		FGuid SaveActorID = Inter->GetActorSaveID_Implementation();
		if (!SaveActorID.IsValid())
		{
			continue;
		}
		FSaveActorData SaveActorData = Inter->GetSaveData_Implementation();

		FMemoryWriter MemoryWriter(SaveActorData.ByteData);
		FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, true);
		Ar.ArIsSaveGame = true;
		Actor->Serialize(Ar);
		
		for (auto ActorComp : Actor->GetComponents())
		{
			if (!ActorComp->Implements<USaveActorInterface>())
			{
				continue;
			}
			ISaveActorInterface* CompInter = Cast<ISaveActorInterface>(ActorComp);
			if (CompInter == nullptr)
			{
				continue;
			}
			// SCD | Save Component Data
			FSaveComponentData SCD = CompInter->GetComponentSaveData_Implementation();
			FMemoryWriter CompMemWriter(SCD.ByteData);
			FObjectAndNameAsStringProxyArchive CAr(CompMemWriter, true);
			CAr.ArIsSaveGame = true;
			ActorComp->Serialize(CAr);
			SCD.ComponentClass = ActorComp->GetClass();

			SaveActorData.ComponentData.Add(SCD);
		}

		SaveableActorData.Add(SaveActorID, SaveActorData);

	}
}

void USurvivalGameInstance::LoadGame()
{
	if (!UGameplayStatics::DoesSaveGameExist(SaveGameName, 0))
	{
		//TODO: Add logging and error message about missing save game
		return;
	}

	SaveableActorData.Empty();
	SaveGameObject = Cast<USurvivalSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveGameName, 0));
	SaveableActorData = SaveGameObject->GetSaveActorData();

	for (TTuple<FGuid, FSaveActorData> SaveActorData : SaveableActorData)
	{
		if (SaveActorData.Value.WasSpawned)
		{
			AActor* SpawnedActor = GetWorld()->SpawnActor(SaveActorData.Value.ActorClass->StaticClass(), &SaveActorData.Value.ActorTransform);
			ISaveActorInterface* Inter = Cast<ISaveActorInterface>(SpawnedActor);
			if (Inter == nullptr)
			{
				continue;
			}
			// Set Actor GUID
		}
	}

	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor) || !Actor->Implements<USaveActorInterface>())
		{
			continue;
		}
		ISaveActorInterface* Inter = Cast<ISaveActorInterface>(Actor);
		if (Inter == nullptr)
		{
			continue;
		}
		FGuid SaveActorID = Inter->GetActorSaveID_Implementation();
		if (!SaveableActorData.Find(SaveActorID))
		{
			continue;
		}
		FSaveActorData SaveActorData = SaveableActorData[SaveActorID];
		Actor->SetActorTransform(SaveActorData.ActorTransform);
		
		FMemoryReader MemoryReader(SaveActorData.ByteData);
		FObjectAndNameAsStringProxyArchive Ar(MemoryReader, true);
		Ar.ArIsSaveGame = true;
		Actor->Serialize(Ar);

		for (auto ActorComp : Actor->GetComponents())
		{
			if (!ActorComp->Implements<USaveActorInterface>())
			{
				continue;
			}
			ISaveActorInterface* CompInter = Cast<ISaveActorInterface>(ActorComp);
			if (CompInter == nullptr)
			{
				continue;
			}
			for (auto SCD : SaveActorData.ComponentData)
			{
				/***************************************************************************
				*This is not safe if an actor has 2 of the same components that are saved  *
				*as the list component returned by Actor->GetComponents() will get all data*
				*																		   *
				* One possible option is a GUID on the component*
				***************************************************************************/


				if (SCD.ComponentClass != ActorComp->GetClass())
				{
					continue;
				}

				FMemoryReader CompMemReader(SCD.ByteData);
				FObjectAndNameAsStringProxyArchive CAr(CompMemReader, true);
				CAr.ArIsSaveGame = true;
				ActorComp->Serialize(CAr);
				if (SCD.RawData.IsEmpty())
				{
					break;
				}
				CompInter->SetComponentSaveData_Implementation(SCD);
				break;
			}
		}
	}
}

void USurvivalGameInstance::AddActorData(const FGuid& ActorID, FSaveActorData ActorData)
{
	SaveableActorData.Add(ActorID, ActorData);
}

FSaveActorData USurvivalGameInstance::GetActorData(const FGuid& ActorID)
{
	return SaveableActorData[ActorID];
}

void USurvivalGameInstance::DEV_LoadGame()
{
	LoadGame();
}

void USurvivalGameInstance::DEV_SaveGame()
{
	if (SaveGameObject == nullptr)
	{
		CreateSaveSlot();
	}
	GatherActorData();
	SaveGameObject->SetSaveActorData(SaveableActorData);
	UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveGameName, 0);
}
