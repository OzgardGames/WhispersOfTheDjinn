// Fill out your copyright notice in the Description page of Project Settings.


#include "WOD_PlayerState.h"
#include "WOD_Character.h"
#include "WOD_PlayerController.h"
#include "BaseAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine.h"

AWOD_PlayerState::AWOD_PlayerState()
{
    bReplicates = true;
    bAlwaysRelevant = true;
}

void AWOD_PlayerState::SetCoopRole(ECoopRole newRole)
{
    CoopRole = newRole;
}

ECoopRole AWOD_PlayerState::GetCoopRole() const
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

void AWOD_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AWOD_PlayerState, CoopRole);
    DOREPLIFETIME(AWOD_PlayerState, PlayerRole);
}
