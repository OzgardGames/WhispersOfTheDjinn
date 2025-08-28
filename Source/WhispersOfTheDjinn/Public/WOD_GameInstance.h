// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WOD_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WHISPERSOFTHEDJINN_API UWOD_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	APlayerController* CreateLocalPlayer(int32 ControllerId, bool bSpawnPawn = true);
};
