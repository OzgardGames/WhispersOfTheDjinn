// Fill out your copyright notice in the Description page of Project Settings.


#include "WOD_PlayerController.h"
#include "WOD_Character.h"
#include "WOD_PlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Lantern.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "InputAction.h"

AWOD_PlayerController::AWOD_PlayerController()
{
    bReplicates = true;
    bAlwaysRelevant = true;
}

void AWOD_PlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void AWOD_PlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    if (GetWorld()->IsNetMode(NM_Standalone))
    {
        ApplyLocalInputMapping();
    }
}

void AWOD_PlayerController::OnRep_PlayerState()
{
    ApplyLocalInputMapping();
}

void AWOD_PlayerController::ApplyLocalInputMapping()
{

    if (!IsLocalController()) return;

    ULocalPlayer* LocalPlayer = GetLocalPlayer();

    if (!LocalPlayer) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!Subsystem) return;

    Subsystem->ClearAllMappings();

    AWOD_PlayerState* PS = LocalPlayer->GetPlayerController(GetWorld())->GetPlayerState<AWOD_PlayerState>();
    if (!PS) return;

    if (PS->GetRole() == ECoopRole::PlayerOne)
    {
        Subsystem->AddMappingContext(IMC_PlayerOne, MappingPriority);
        UE_LOG(LogTemp, Log, TEXT("Player 1 Received Mapping system %s."),*IMC_PlayerOne->GetName());
    }
    else if (PS->GetRole() == ECoopRole::PlayerTwo)
    {
        Subsystem->AddMappingContext(IMC_PlayerTwo, MappingPriority);
        UE_LOG(LogTemp, Log, TEXT("Player 2 Received Mapping system %s."), *IMC_PlayerTwo->GetName());
    }
}


