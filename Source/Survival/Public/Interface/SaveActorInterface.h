// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveActorInterface.generated.h"


USTRUCT(BlueprintType)
struct FSaveActorData
{
	GENERATED_USTRUCT_BODY()
	// No private with getters and setters because it is short term
	// and because it is a singleplayer game, if MP, you'd make them private with getters/setters
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTransform ActorTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<uint8> ByteData;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool WasSpawned = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UClass* ActorClass;
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SURVIVAL_API ISaveActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
