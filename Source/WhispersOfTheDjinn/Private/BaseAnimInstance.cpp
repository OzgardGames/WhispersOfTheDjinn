// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"


void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	AWOD_Character* Character = Cast<AWOD_Character>(TryGetPawnOwner());

	if (Character)
	{
		CurrentAnimState = Character->AnimState;
		aIsCrouching = Character->bIsCrouched;
		aIsHanging = Character->bIsHanging;
	}

}
