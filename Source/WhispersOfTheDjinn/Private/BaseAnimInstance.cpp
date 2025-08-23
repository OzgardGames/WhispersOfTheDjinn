// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"
#include "WOD_Character.h"
#include "WOD_PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

void UBaseAnimInstance::SetAnimState(ECharacterAnimState newAnimState)
{
	AnimState = newAnimState;
}

ECharacterAnimState UBaseAnimInstance::GetAnimState()
{
	return AnimState;
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

}

ECharacterAnimState UBaseAnimInstance::MapPlayerStateToAnim(EPlayerState State)
{
	switch (State)
	{
	case EPlayerState::Idle:         return ECharacterAnimState::Idle;
	case EPlayerState::Walking:      return ECharacterAnimState::Walking;
	case EPlayerState::Running:      return ECharacterAnimState::Running;
	case EPlayerState::Jumping:      return ECharacterAnimState::Jumping;
	case EPlayerState::Crouching:    return ECharacterAnimState::Crouching;
	case EPlayerState::Carrying:     return ECharacterAnimState::Carrying;
	
		// Add all mappings
	default:                         return ECharacterAnimState::Idle;
	}
}

void UBaseAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBaseAnimInstance, AnimState);
}