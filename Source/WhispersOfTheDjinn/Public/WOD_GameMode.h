// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WOD_Character.h"
#include "GameFramework/GameMode.h"
#include "WOD_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class WHISPERSOFTHEDJINN_API AWOD_BaseGameMode : public AGameMode
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	TArray<AActor*> PlayerStarts;
	int32 JoinCounter = 0;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AWOD_Character> BPCharacter;
};
