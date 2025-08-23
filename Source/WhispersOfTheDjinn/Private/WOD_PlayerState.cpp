// Fill out your copyright notice in the Description page of Project Settings.


#include "WOD_PlayerState.h"
#include "WOD_Character.h"
#include "WOD_PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AWOD_PlayerState::AWOD_PlayerState()
{
    bReplicates = true;
    bAlwaysRelevant = true;
}

void AWOD_PlayerState::SetRole(ECoopRole newRole)
{
    CoopRole = newRole;
}

ECoopRole AWOD_PlayerState::GetRole() const
{
    return CoopRole;
}

void AWOD_PlayerState::SetPlayerRole(EPlayerRole newPlayerRole)
{
    PlayerRole = newPlayerRole;
}

EPlayerRole AWOD_PlayerState::GetPlayerRole() const
{
    return PlayerRole;
}

void AWOD_PlayerState::SetState(EPlayerState newState)
{
    if (PlayerState == newState) return;

    PlayerState = newState;
    OnPlayerStateChanged.Broadcast(PlayerState);

	AWOD_Character* Character = Cast<AWOD_Character>(GetPawn());

    if (!Character) return;

    if (PlayerState == EPlayerState::Idle)
    {
        if (PlayerRole == EPlayerRole::Sister)
        {
            Character->GetCharacterMovement()->MaxWalkSpeed = 450.0f;
        }
        else
        {
            Character->GetCharacterMovement()->MaxWalkSpeed = 550.0f;
        }
		Character->bCanCrouch = true;
    }
    else if (PlayerState == EPlayerState::Walking)
    {

    }
    else if (PlayerState == EPlayerState::Running)
    {

    }
    else if (PlayerState == EPlayerState::Crouching)
    {
        Character->GetCharacterMovement()->MaxWalkSpeed = 300.0f;
		Character->bCanCrouch = false;
    }
    else if (PlayerState == EPlayerState::Carrying)
    {

    }

}

EPlayerState AWOD_PlayerState::GetState() const
{
    return PlayerState;
}

void AWOD_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AWOD_PlayerState, CoopRole);
    DOREPLIFETIME(AWOD_PlayerState, PlayerRole);
    DOREPLIFETIME(AWOD_PlayerState, PlayerState);
}
