// Fill out your copyright notice in the Description page of Project Settings.


#include "WOD_GameInstance.h"
#include "Kismet/GameplayStatics.h"

APlayerController* UWOD_GameInstance::CreateLocalPlayer(int32 ControllerId, bool bSpawnPawn)
{
	return UGameplayStatics::CreatePlayer(GetWorld(), ControllerId, bSpawnPawn);
}
