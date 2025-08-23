// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WOD_PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "WOD_GameMode.generated.h"

class AWOD_Character;
/**
 * 
 */
UCLASS()
class WHISPERSOFTHEDJINN_API AWOD_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWOD_GameMode();

	virtual void BeginPlay() override;
	virtual void RestartPlayer(AController* NewPlayer) override;

protected:

	UFUNCTION()
	void PossessPawns(TArray<AWOD_PlayerController*> playerControllers);

private:
	int PlayerCount = 0;
	int NumPlayers = 2;
	TArray<AActor*> PlayerStarts;
	TArray<AWOD_PlayerController*> PlayerControllers;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AWOD_Character> BPCharacter;

	
};
