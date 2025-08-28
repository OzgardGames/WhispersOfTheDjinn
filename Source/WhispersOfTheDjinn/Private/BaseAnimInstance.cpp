// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"
#include "WOD_Character.h"
#include "WOD_PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

void UBaseAnimInstance::SetAnimState(EPlayerState newAnimState)
{
	AnimState = newAnimState;
}

EPlayerState UBaseAnimInstance::GetAnimState()
{
	return AnimState;
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* Pawn = TryGetPawnOwner();

	if (!Pawn) return;

	AWOD_PlayerState* PS = Pawn->GetPlayerState<AWOD_PlayerState>();

	if (PS)
	{
		AnimState = PS->GetState();
	}

}
