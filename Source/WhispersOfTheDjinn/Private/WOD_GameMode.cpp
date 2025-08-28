// Fill out your copyright notice in the Description page of Project Settings.


#include "WOD_GameMode.h"
#include "WOD_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void AWOD_BaseGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	if (IsNetMode(NM_Standalone))
	{
		// Create a second local player for split-screen
		UWOD_GameInstance* GI = Cast<UWOD_GameInstance>(GetGameInstance());
		GI->CreateLocalPlayer(1, true);
	}

}

void AWOD_BaseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("New player logged in: %s"), *GetNameSafe(NewPlayer));
	JoinCounter++;
	// Now PlayerState is safe to access
	if (NewPlayer->PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s 's PlayerState Created!") , *GetNameSafe(NewPlayer));
	}

	AActor* PStart = FindPlayerStart(NewPlayer);
	FVector SpawnLoc = PStart->GetActorLocation();
	FRotator SpawnRot = PStart->GetActorRotation();

	//Spawn a new pawn for the controller
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APawn* PawnToPossess = GetWorld()->SpawnActor<AWOD_Character>(BPCharacter, SpawnLoc, SpawnRot, SpawnParams);

	if (PawnToPossess)
	{

		if (AWOD_PlayerState* NewPS = NewPlayer->GetPlayerState<AWOD_PlayerState>())
		{
			if (JoinCounter - 1 == 0)
			{
				NewPS->SetRole(ECoopRole::PlayerOne);
				NewPS->SetPlayerRole(EPlayerRole::Sister);
				UE_LOG(LogTemp, Log, TEXT("Player %d spawned with Role : %s."), JoinCounter + 1, *UEnum::GetDisplayValueAsText(NewPS->GetPlayerRole()).ToString());

			}
			else
			{
				NewPS->SetRole(ECoopRole::PlayerTwo);
				NewPS->SetPlayerRole(EPlayerRole::Brother);
				UE_LOG(LogTemp, Log, TEXT("Player %d spawned with Role : %s."), JoinCounter + 1, *UEnum::GetDisplayValueAsText(NewPS->GetPlayerRole()).ToString());

			}
		}

		NewPlayer->Possess(PawnToPossess);
	}

}
