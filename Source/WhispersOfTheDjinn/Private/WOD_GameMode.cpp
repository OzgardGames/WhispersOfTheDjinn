// Fill out your copyright notice in the Description page of Project Settings.


#include "WOD_GameMode.h"
#include "WOD_Character.h"
#include "WOD_PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Net/UnrealNetwork.h"


AWOD_GameMode::AWOD_GameMode()
{
    DefaultPawnClass = nullptr;
}

void AWOD_GameMode::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("AWOD_GameMode BeginPlay running!"));

    // Starting the game locally on the same computer
    if (GetWorld()->IsNetMode(NM_Standalone))
    {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
    
        PlayerControllers.Add(Cast<AWOD_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)));
        PlayerControllers.Add(Cast<AWOD_PlayerController>(UGameplayStatics::CreatePlayer(GetWorld(), -1, true)));

        PossessPawns(PlayerControllers);
    }

    UE_LOG(LogTemp, Warning, TEXT("GM: BeginPlay on server? %s"), HasAuthority() ? TEXT("YES (server)") : TEXT("NO (client)"));

}

void AWOD_GameMode::RestartPlayer(AController* NewPlayer)
{
    UE_LOG(LogTemp, Warning, TEXT("GM: RestartPlayer (%s)"), *GetNameSafe(NewPlayer));
    Super::RestartPlayer(NewPlayer);

    if (!GetWorld()->IsNetMode(NM_Standalone))
    {
        if (!NewPlayer) return;

        AActor* StartLoc = ChoosePlayerStart(NewPlayer);

        FVector SpawnLoc = StartLoc->GetActorLocation();
        FRotator SpawnRot = StartLoc->GetActorRotation();

        AWOD_PlayerState* PS = Cast<AWOD_PlayerState>(NewPlayer->PlayerState);

        if (!PS) return;

        if (PlayerCount == 0)
        {
            PS->SetRole(ECoopRole::PlayerOne);
            PS->SetPlayerRole(EPlayerRole::Sister);
            UE_LOG(LogTemp, Log, TEXT("Player 1 spawned and possessed successfully, with Role : Sister."));
        }
        else
        {
            PS->SetRole(ECoopRole::PlayerTwo);
            PS->SetPlayerRole(EPlayerRole::Brother);
            UE_LOG(LogTemp, Log, TEXT("Player 2 spawned and possessed successfully, with Role : Brother."));
        }

        if (NewPlayer->IsLocalController())
        {
            if (AWOD_PlayerController* WPC = Cast<AWOD_PlayerController>(NewPlayer))
            {
                WPC->ApplyLocalInputMapping();
            }
        }

        if (BPCharacter)
        {
            FActorSpawnParameters Params;
            Params.Owner = NewPlayer;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            APawn* newPawn = GetWorld()->SpawnActor<APawn>(BPCharacter, SpawnLoc, SpawnRot, Params);

            if (newPawn)
            {
                NewPlayer->Possess(newPawn);
                UE_LOG(LogTemp, Warning, TEXT("GM: HandleStartingNewPlayer (%s)"), *GetNameSafe(NewPlayer));
            }
        }

        PlayerCount++;
    }
}

void AWOD_GameMode::PossessPawns(TArray<AWOD_PlayerController*> playerControllers)
{
    for (int i = 0; i < NumPlayers; i++)
    {
        //Pick a Player Start Position
        FVector SpawnLocation = PlayerStarts[i]->GetActorLocation();
        FRotator SpawnRotation = PlayerStarts[i]->GetActorRotation();
        
        //Spawn a new pawn for the controller
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        APawn* PawnToPossess = GetWorld()->SpawnActor<AWOD_Character>(BPCharacter, SpawnLocation, SpawnRotation, SpawnParams);
        
        if (PawnToPossess)
        {
            AWOD_PlayerState* PS = playerControllers[i]->GetPlayerState<AWOD_PlayerState>();

            if (i == 0)
            {
                PS->SetRole(ECoopRole::PlayerOne);
                PS->SetPlayerRole(EPlayerRole::Sister);
            }
            else
            {
                PS->SetRole(ECoopRole::PlayerTwo);
                PS->SetPlayerRole(EPlayerRole::Brother);
            }

            playerControllers[i]->Possess(PawnToPossess);
            UE_LOG(LogTemp, Log, TEXT("Player %d spawned and possessed successfully, with Role : %s."), i + 1, *UEnum::GetDisplayValueAsText(PS->GetPlayerRole()).ToString());
        }
    }
}

