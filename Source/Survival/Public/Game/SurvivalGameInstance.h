// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interface/SaveActorInterface.h"
#include "SurvivalGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class SURVIVAL_API USurvivalGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
private:

	TMap<FGuid, FSaveActorData> SaveableActorData;
	UPROPERTY()
	class USurvivalSaveGame* SaveGameObject = nullptr;
	FString SaveGameName = TEXT("DEFAULT");
	FName CurrentlyLoadedLevel = "NONE";

	USurvivalGameInstance();

	void CreateSaveSlot();
	void GatherActorData();

public:

	UFUNCTION(BlueprintCallable)
	void AddActorData(const FGuid& ActorID, FSaveActorData ActorData);
	UFUNCTION(BlueprintCallable)
	FSaveActorData GetActorData(const FGuid& ActorID);
};
